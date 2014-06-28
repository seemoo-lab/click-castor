/**************
 * Parameters *
 **************/

define(
	$EthDev eth0, /** eth0 for ns3 **/
	$HostDev tap0, /** tap0 for ns3 **/
	$CASTORTYPE	201);

AddressInfo(fake $EthDev);

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
		-> ethdev :: ToSimDevice($myEthDev);
}

/**
 * Handle incoming packets on Ethernet device
 */
elementclass InputEth{
	$myEthDev, $myAddr |

	ethdev :: FromSimDevice($myEthDev, SNAPLEN 4096)
		-> HostEtherFilter($myEthDev)
		-> arpclassifier :: Classifier(12/0806 20/0001, 12/0806 20/0002, -); // Filter ARP messages (Request / Replay / Default)

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
	$myHostDev |

	fromhost :: FromSimDevice($myHostDev, SNAPLEN 4096)
		-> CheckIPHeader2
		-> MarkIPHeader
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
	-> CastorEncryptACKAuthDummy($crypto, $myIP)
	-> CastorPrint('Send', $myIP)
	-> output;
}

elementclass CastorClassifier{

	input
		-> CheckIPHeader
		-> ipclassifier :: IPClassifier(ip proto $CASTORTYPE, -)
		-> GetIPAddress(12)
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
		-> validateAtDest :: CastorValidateFlowAtDestination
		-> CastorAddToHistory($history, false)
		-> genAck :: CastorCreateACKDummy($crypto)
		-> [0]output;

	genAck[1] // Generate ACK for received PKT
		-> CastorPrint('Generated', $myIP)
		-> CastorAddToHistory($history, false)
		-> IPEncap($CASTORTYPE, $myIP, 255.255.255.255)
		-> [1]output; // Push ACKs to output 1

	// If invalid -> discard
	null :: Discard;
	validateAtDest[1]
		-> null;

}

elementclass CastorForwardPKT {
	$myIP, $routingtable, $history |

	input
		-> CastorPrint('Forwarding Packet', $myIP)
		-> CastorLookupRoute($routingtable)		// Lookup the route for the packet
		-> CastorAddToHistory($history, true)
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
		-> checkDuplicate :: CastorCheckDuplicate($history)
		-> validate :: CastorValidateFlow
		-> cdst :: CastorDstClassifier($myIP);

 	// PKT arrived at destination
	cdst[0]
		-> handleLocal :: CastorLocalPKT($myIP, $history, $crypto)
		-> [0]output;

	handleLocal[1]
		-> [1]output;
	
	// PKT needs to be forwarded
	cdst[1]
		-> forward :: CastorForwardPKT($myIP, $routingtable, $history)
		-> [2]output;

	// If invalid or duplicate -> discard
	null :: Discard;
	checkDuplicate[1]
		-> CastorPrint("Duplicate", $myIP)
		-> null;
	validate[1]
		-> null;

}

elementclass CastorHandleACK{
	$myIP, $routingtable, $history, $crypto |

	// Regular ACK flow
	input
		// TODO maybe swap 'validate' and 'checkDuplicate'
		-> validate :: CastorValidateACK($history)
		-> checkDuplicate :: CastorCheckDuplicate($history)
		-> updateEstimates :: CastorUpdateEstimates($routingtable, $history)
		-> CastorAddToHistory($history, false)
		-> CastorPrint('Received', $myIP)
		-> IPEncap($CASTORTYPE, $myIP, 255.255.255.255)
		-> output;

	// If invalid or duplicate -> discard
	null :: Discard;
	validate[1]
		-> CastorPrint("Invalid", $myIP)
		-> null;
	checkDuplicate[1]
		-> CastorPrint("Duplicate", $myIP)
		-> null;
	updateEstimates[1]
		-> CastorPrint("Could not update estimates", $myIP)
		-> null; // TODO why is there a discard in update estimates?
}


/*************************
 * Initialize the Blocks *
 *************************/

ethin :: InputEth($EthDev, fake);
ethout :: OutputEth($EthDev);
fromhost :: FromHost($HostDev);
tohost :: ToHost($HostDev);

sam::SAManagement(fake);
crypto::Crypto(sam);
flowDB :: CastorFlowStub;
flow_merkle :: CastorFlowMerkle(flowDB, crypto);
routingtable :: CastorRoutingTable;
history :: CastorHistory(crypto,timeout);
timeout :: CastorTimeout(routingtable,history);
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
