/**************
 * Parameters *
 **************/

define(
	/** Device names for ns3 **/
	$EthDev eth0,
	$HostDev tap0,

	$CASTORTYPE 201,

	/** Castor parameters (settings from experimental setup of Castor technical report) **/
	$broadcastAdjust 8.0, // bandwidth investment for route discovery (larger values reduce the broadcast probability)
	$updateDelta 0.8, // adaptivity of the reliability estimators
	$timeout 500, // in milliseconds

	$jitter 100, // jitter in microseconds to avoid collisions for broadcast traffic
	
	$maxGroupSize 10, // how many destinations per Xcast PKT?
);

AddressInfo(fake $EthDev);
AddressInfo(netAddr 192.168.201.0)

// Need an IP routing table for ns-3 (dummy)
rt :: StaticIPLookup(0.0.0.0/0 0);
Idle() -> rt -> Discard;


/****************
 * Input/Output *
 ****************/

/**
 * Handle output to the Ethernet device
 */
elementclass OutputEth{ 
	$myEthDev |

	input[0]
		-> Queue
		-> JitterUnqueue($jitter)
		-> ethdev :: ToSimDevice($myEthDev);
}

/**
 * Handle incoming packets on Ethernet device
 */
elementclass InputEth{
	$myEthDev, $myAddr |

	ethdev :: FromSimDevice($myEthDev, SNAPLEN 4096)
		-> HostEtherFilter($myEthDev)
		-> arpclassifier :: Classifier(12/0806 20/0001, 12/0806 20/0002, -); // Filter ARP messages (Request / Reply / Default)

	arpclassifier[0] // Handle ARP request
		-> ARPResponder($myAddr)
		-> [1]output;

	arpclassifier[1] // Handle ARP response
		-> [0]output;

	arpclassifier[2] // Handle default
		-> [2]output;

}

/**
 * Handle packets destined for local host
 *
 * Input(0): Castor PKT
 * Input(1): Other packet
 */
elementclass ToHost {
	$myHostDev |
	
	hostdevice :: ToSimDevice($myHostDev, IP);

	input[0]
		-> CastorXcastRemoveHeader($maxGroupSize)
		-> CheckIPHeader2
		-> MarkIPHeader
		-> hostdevice;

	input[1]
		-> hostdevice;

}

/**
 * Handle incoming packets from local host
 */
elementclass FromHost {
	$myHostDev, $myIP |

	fromhost :: FromSimDevice($myHostDev, SNAPLEN 4096)
		-> CheckIPHeader2
		-> MarkIPHeader
		-> CastorTranslateLocalhost($myIP) // Packets coming from host have 127.0.0.1 set as source address, so replace with address of local host
		-> output;

}


/**********
 * CASTOR *
 **********/

/**
 * Transforms IP packet (from local host) to valid Castor PKT
 */
elementclass CastorHandleIpPacket{
	$myIP, $flowDB, $crypto |

	map :: CastorXcastDestinationMap

	input
	-> CastorXcastSetFixedHeader($flowDB, $maxGroupSize)
	-> CastorXcastSetDestinations($crypto, map)
	-> CastorPrint('Send', $myIP)
	-> output;
}

elementclass CastorClassifier{

	input
		-> MarkIPHeader
		-> CheckIPHeader
		-> ipclassifier :: IPClassifier(ip proto $CASTORTYPE, -)
		-> annotateSenderAddress :: GetIPAddress(12)
		-> StripIPHeader
		-> cclassifier :: Classifier(0/c?, 0/a?);

	cclassifier[0] // Castor PKTs -> output 0
		-> [0]output;

	cclassifier[1] // Castor ACKs -> output 1
		-> [1]output;

	ipclassifier[1] // Other packets -> output 2
		-> [2]output;

}

elementclass CastorLocalXcastPkt {
	$myIP, $history, $crypto |

	input
		-> CastorXcastAnnotateAckAuth($crypto)
		-> validateAtDest :: CastorValidateFlowAtDestination($crypto)
		-> CastorPrint('Packet arrived at destination', $myIP)
		-> CastorAddXcastPktToHistory($history)
		-> genAck :: CastorXcastCreateAck($myIP)
		-> [0]output;

	genAck[1] // Generate ACK for received PKT
		-> CastorPrint('Generated', $myIP)
		-> CastorAddAckToHistory($crypto,$history)
		-> IPEncap($CASTORTYPE, $myIP, 255.255.255.255)
		-> [1]output; // Push ACKs to output 1

	// If invalid -> discard
	null :: Discard;
	validateAtDest[1]
		-> CastorPrint("Packet authentication failed", $myIP)
		-> null;

}

elementclass CastorForwardXcastPkt {
	$myIP, $routingtable, $history |

	input
		//-> CastorPrint('Forwarding Packet', $myIP)
		-> CastorXcastLookupRoute($routingtable)		// Lookup the route for the packet
		-> CastorAddXcastPktToHistory($history)
		-> CastorTimeout($routingtable,$history,$timeout,$myIP)
		-> IPEncap($CASTORTYPE, $myIP, DST_ANNO)	// Encapsulate in a new IP Packet
		-> output;

}

/**
 * Input:	Castor PKT
 * Output(0):	PKT for local host
 * Output(1):	New ACK
 * Output(2):	Forwarded PKT
 */
elementclass CastorHandleXcastPkt{
	$myIP, $routingtable, $history, $crypto |

	input
		-> forwarderClassifier :: CastorXcastForwarderClassifier($myIP)
		-> checkDuplicate :: CastorXcastCheckDuplicate($history, $myIP)
		-> validate :: CastorXcastValidateFlow($crypto)
		-> destinationClassifier :: CastorXcastDestClassifier($myIP);

 	// PKT arrived at destination
	destinationClassifier[0]
		-> handleLocal :: CastorLocalXcastPkt($myIP, $history, $crypto)
		-> [0]output;

	handleLocal[1]
		-> [1]output;
	
	// PKT needs to be forwarded
	destinationClassifier[1]
		-> forward :: CastorForwardXcastPkt($myIP, $routingtable, $history)
		-> [2]output;

	// If invalid or duplicate -> discard
	null :: Discard;
	forwarderClassifier[1]
		//-> CastorPrint("Node not in forwarder list", $myIP)
		-> null;
	checkDuplicate[1]
		//-> CastorPrint("Duplicate", $myIP)
		-> null;
	validate[1]
		-> CastorPrint("Flow authentication failed", $myIP)
		-> null;

}

elementclass CastorHandleXcastAck{
	$myIP, $routingtable, $history, $crypto |

	// Regular ACK flow
	input
		-> calcPid :: CastorAnnotatePid($crypto)
		-> validate :: CastorValidateACK($crypto, $history)
		-> updateEstimates :: CastorUpdateEstimates($crypto, $routingtable, $history)
		-> CastorAddAckToHistory($crypto, $history)
		//-> CastorPrint('Received valid', $myIP)
		-> IPEncap($CASTORTYPE, $myIP, 255.255.255.255)
		-> output;

	// Discarding...
	null :: Discard;
	validate[1]
		//-> CastorPrint("Unknown corresponding PKT", $myIP)
		-> null;
	validate[2]
		-> CastorPrint("Too late", $myIP)
		-> null;
	validate[3]
		//-> CastorPrint("Duplicate from same neighbor", $myIP)
		-> null;
	updateEstimates[1]
		//-> CastorPrint("Duplicate", $myIP)
		-> null;
	updateEstimates[2]
		//-> CastorPrint("Received from wrong neighbor", $myIP)
		-> null;
}


/*************************
 * Initialize the Blocks *
 *************************/

ethin :: InputEth($EthDev, fake);
ethout :: OutputEth($EthDev);
fromhost :: FromHost($HostDev, fake);
tohost :: ToHost($HostDev);

sam::SAManagement(fake, netAddr, 100);
crypto::Crypto(sam);
flowDB :: CastorFlowStub;
flow_merkle :: CastorFlowMerkle(flowDB, crypto);
routingtable :: CastorRoutingTable($broadcastAdjust, $updateDelta);
history :: CastorHistory;
castorclassifier :: CastorClassifier;
handlepkt :: CastorHandleXcastPkt(fake, routingtable, history, crypto);
handleack :: CastorHandleXcastAck(fake, routingtable, history, crypto);

handleIpPacket :: CastorHandleIpPacket(fake, flowDB, crypto);
arpquerier :: ARPQuerier(fake, TIMEOUT 3600); // Set timeout sufficiently long, so we don't introduce ARP overhead (we set entries in ns-3)


/*******************
 * Wire the Blocks *
 *******************/

ethin[1] -> ethout;			// Push new ARP Responses back to device
ethin[0] -> [1]arpquerier;	// Push incoming arp responses to querer
ethin[2]
	-> removeEthernetHeader :: Strip(14)
 	-> castorclassifier;	// Classify received packets			
 
arpquerier -> ethout;	// Send Ethernet packets to output

fromhost	
	-> handleIpPacket 
	-> handlepkt;		// Process new generated packets
 
castorclassifier[0] -> handlepkt; // Process PKTs
castorclassifier[1] -> handleack; // Process ACKs
castorclassifier[2] -> [1]tohost; // Deliver non-Castor packets directly to host

handlepkt[0]		-> [0]tohost;  // Deliver PKT to host
handlepkt[1]		-> arpquerier; // Return ACK		
handlepkt[2]		-> arpquerier; // Forward PKT
handleack		-> arpquerier; // Forward ACK