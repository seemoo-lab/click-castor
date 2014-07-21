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
	$timeout 500, // in msec

	$jitter 100, // jitter in microseconds to avoid collisions for broadcast traffic
);

AddressInfo(fake $EthDev);
AddressInfo(netAddr 192.168.201.0)

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
		-> JitterUnqueue($jitter) // Jitter in microseconds
		-> ethdev :: ToSimDevice($myEthDev);
}

/**
 * Handle incoming packets on Ethernet device
 */
elementclass InputEth{
	$myEthDev, $myAddr |

	ethdev :: FromSimDevice($myEthDev, SNAPLEN 4096)
		-> ToDump(ethin,PER_NODE 1)
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
		-> CastorRemoveHeader
		-> CheckIPHeader
		-> MarkIPHeader
		-> EtherEncap(0x0800, 1:1:1:1:1:1, ff:ff:ff:ff:ff:ff)
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
		-> CastorTranslateLocalhost($myIP) // Packets coming from host have 127.0.0.1 set as source address, so replace with address of 
		-> output;

}


/**********
 * CASTOR *
 **********/

/**
 * Transforms IP packet (from local host) to valid Castor PKT
 */
elementclass CastorHandleIPPacket{
	$myIP, $flowDB, $crypto |

	-> CastorAddHeader($flowDB)
	-> CastorEncryptACKAuth($crypto)
	-> CastorPrint('Send', $myIP)
	-> output;
}

elementclass CastorClassifier{

	input
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

elementclass CastorLocalPKT {
	$myIP, $history, $crypto |

	input
		-> CastorPrint('Packet arrived at destination', $myIP)
		-> CastorDecryptACKAuth($crypto)
		-> validateAtDest :: CastorValidateFlowAtDestination($crypto)
		-> CastorAddXcastPktToHistory($history)
		-> genAck :: CastorXcastCreateAck
		-> [0]output;

	genAck[1] // Generate ACK for received PKT
		-> CastorPrint('Generated', $myIP)
		-> CastorAddACKToHistory($crypto,$history)
		-> IPEncap($CASTORTYPE, $myIP, 255.255.255.255)
		-> [1]output; // Push ACKs to output 1

	// If invalid -> discard
	null :: Discard;
	validateAtDest[1]
		-> CastorPrint("!!! Invalid @ destination", $myIP)
		-> null;

}

elementclass CastorForwardPKT {
	$myIP, $routingtable, $history |

	input
		-> CastorPrint('Forwarding Packet', $myIP)
		-> CastorLookupRoute($routingtable)		// Lookup the route for the packet
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
elementclass CastorHandlePKT{
	$myIP, $routingtable, $history, $crypto |

	input
		-> forwarderClassifier :: CastorXcastForwarderClassifier($myIP)
		-> checkDuplicate :: CastorCheckDuplicate($history)
		-> validate :: CastorXcastValidateFlow($crypto)
		-> destinationClassifier :: CastorXcastDestClassifier($myIP);

 	// PKT arrived at destination
	destinationClassifier[0]
		-> handleLocal :: CastorLocalPKT($myIP, $history, $crypto)
		-> [0]output;

	handleLocal[1]
		-> [1]output;
	
	// PKT needs to be forwarded
	destinationClassifier[1]
		-> forward :: CastorForwardPKT($myIP, $routingtable, $history)
		-> [2]output;

	// If invalid or duplicate -> discard
	null :: Discard;
	forwarderClassifier[1]
		-> CastorPrint("Node not in forwarder list", $myIP)
		-> null;
	checkDuplicate[1]
		-> CastorPrint("Duplicate", $myIP)
		-> null;
	validate[1]
		-> CastorPrint("!!! Invalid", $myIP)
		-> null;

}

elementclass CastorHandleACK{
	$myIP, $routingtable, $history, $crypto |

	// Regular ACK flow
	input
		-> calcPid :: CastorAnnotatePid($crypto)
		-> validate :: CastorValidateACK($crypto, $history)
		-> updateEstimates :: CastorUpdateEstimates($crypto, $routingtable, $history)
		-> CastorAddACKToHistory($crypto, $history)
		-> CastorPrint('Received valid', $myIP)
		-> IPEncap($CASTORTYPE, $myIP, 255.255.255.255)
		-> output;

	// Discarding...
	null :: Discard;
	validate[1]
		-> CastorPrint("Unknown corresponding PKT", $myIP)
		-> null;
	validate[2]
		-> CastorPrint("Too late", $myIP)
		-> null;
	validate[3]
		-> CastorPrint("Duplicate from same neighbor", $myIP)
		-> null;
	updateEstimates[1]
		-> CastorPrint("Duplicate", $myIP)
		-> null;
	updateEstimates[2]
		-> CastorPrint("Received from wrong neighbor", $myIP)
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
handlepkt :: CastorHandlePKT(fake, routingtable, history, crypto);
handleack :: CastorHandleACK(fake, routingtable, history, crypto);

handleIPPacket :: CastorHandleIPPacket(fake, flowDB, crypto);
arpquerier :: ARPQuerier(fake);


/*******************
 * Wire the Blocks *
 *******************/

ethin[1] -> ethout;		// PUSH new ARP Responses back to device
ethin[0] -> [1]arpquerier;	// Push incoming arp responses to querer
ethin[2]
	-> removeEthernetHeader :: Strip(14)
 	-> castorclassifier;	// Classify received packets			
 
arpquerier -> ethout; // Send Ethernet packets to output

fromhost	
	-> handleIPPacket 
	-> handlepkt;		// Process new generated packets
 

castorclassifier[0] -> handlepkt; // Process PKTs
castorclassifier[1] -> handleack; // Process ACKs
castorclassifier[2] -> [1]tohost; // Deliver non-Castor packets directly to host

handlepkt[0]		-> [0]tohost;  // Deliver PKT to host
handlepkt[1]		-> arpquerier; // Return ACK		
handlepkt[2] 		-> arpquerier; // Forward PKT
handleack 		-> arpquerier; // Forward ACK