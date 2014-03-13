/*
 * Parameters
 */

define(
	$MyETH 		eth1,
	$CASTORTYPE	201);

AddressInfo(
	fake	XXX.XXX.XXX.XXX/XX
);

/**
 *	Setup some blocks
 */

// Handle the Output to the Ethernet device
elementclass Output{ 
	$mydev |

	input[0]
		-> Queue
		-> ToDevice($mydev);
}

// Handle incoming packets on Ethernet device
elementclass InputEth{
	$myaddr, $myaddr_ethernet |

	FromDevice($myaddr_ethernet)
		-> HostEtherFilter($myaddr_ethernet)
		-> arpclassifier :: Classifier(12/0806 20/0001, 12/0806 20/0002, -);	// Filter ARP Messages (Request / Replay / Default)
	//Handle ARP Requests
	arpclassifier[0]
		-> ARPResponder($myaddr $myaddr_ethernet)
	//	-> ARPPrint(LABEL "we arp responded")
		-> [1]output;

	//Handle ARP Response
	arpclassifier[1]
	//	-> ARPPrint(LABEL "In Response")
		-> [0]output;

	//Handle Default
	arpclassifier[2]
	//	-> Print(LABEL "Received not ARP Message")
		-> [2]output;
}

// Handle incoming packets on fake device
elementclass System{
	$myaddr |

	tohost :: ToHost(fake0);
	
	FromHost(fake0, $myaddr)
		-> fromhost_cl :: Classifier(12/0806, 12/0800);
	fromhost_cl[0] 
		-> ARPResponder(0.0.0.0/0 1:1:1:1:1:1) 
		-> ToHost(fake0);
	fromhost_cl[1]
		//-> Print(LABEL "got sth from host", MAXLENGTH -1)
		-> Strip(14)
		-> CheckIPHeader
		-> MarkIPHeader
		-> IPPrint(LABEL "Transmitting Packet over CASTOR")
		-> output;

	input[0]
		-> CastorRemoveHeader
		-> CheckIPHeader()
		-> MarkIPHeader()
		-> IPPrint("Received at destination: ")
		-> EtherEncap(0x0800, 1:1:1:1:1:1, ff:ff:ff:ff:ff:ff)
		-> tohost;
}

// Create some random Castor Packets for testing
elementclass CastorSource{

	src :: TimedSource(DATA \<
		//IP Header
		45 00 00 28  00 00 00 00  40 11 77 c3  01 00 00 01 01 00 00 03 
		// IP Payload
		13 69 13 69  00 14 d6 41  55 44 50 20
		70 61 63 6b  65 74 21 0a>, INTERVAL 0.333, LIMIT 4, STOP false);

	src -> SetIPChecksum
		-> CheckIPHeader
		-> output;
}

elementclass HandleIPPacket{
	$flowDB, $crypto, $myIP |

	-> CastorAddHeader($flowDB)
	-> CastorEncryptACKAuthDummy($crypto, $myIP)
	-> output;
}


elementclass CastorClassifier{
	input
		-> CheckIPHeader
		-> ipclassifier :: IPClassifier(ip proto $CASTORTYPE, -)
		-> neighborfilter :: IPClassifier( ##FILT## , -)
		-> GetIPAddress(12)
		-> StripIPHeader
		-> cclassifier :: Classifier(0/c?, 0/a?)
		//Push Castor Packets to Output 0
		-> [0]output;

	// Push Castor ACKs to Output 1
	cclassifier[1]
		-> [1]output;

	// Push non Castor Packets to Output 2
	ipclassifier[1]
		-> [2]output;
		
	// Drop Packets from not allowed Neighbors, used for evaluation only
	neighborfilter[1]
		-> Discard();
}

elementclass CastorHandlePKT{
	$myip, $routingtable, $history, $crypto |

	input
		-> CastorCheckDuplicate($history)
		-> cdst :: CastorDstClassifier($myip);

	//Process Packets for me
	cdst[0]
		-> CastorPrint('CASTOR arrived at destination')
		-> CastorValidateFlow				// Validate the Flow of the Castor
		-> CastorAddToHistory($history)
		-> genAck :: CastorCreateACKDummy($crypto)		// Generate a new ACK
		-> [0]output;					// Push Packets for Host to Output 0

	//Broadcast a new ACK
	genAck[1]
		-> CastorPrint('Generated')
		-> CastorAddToHistory($history)
		-> IPEncap($CASTORTYPE, $myip, 255.255.255.255)
		-> [2]output;						// Push Acks to Output 2
	
	//Forward other packets
	cdst[1]
		-> CastorPrint('Forwarding Packet ...')
		-> CastorLookupRoute($routingtable)		// Lookup the route for the packet
		-> CastorAddToHistory($history)
		-> IPEncap($CASTORTYPE, $myip, DST_ANNO)	// Encapsulate in a new IP Packet
		//-> IPPrint('Encapped: ')
		-> [1]output;
}

elementclass CastorHandleACK{
	$myip, $routingtable, $history, $crypto |

	input
		-> CastorCheckDuplicate($history)
		-> CastorUpdateEstimates($routingtable, $history, $crypto)
		-> CastorAddToHistory($history)
		-> CastorPrint('Received')
		-> IPEncap($CASTORTYPE, $myip, 255.255.255.255)
		-> output;
}


/*
 *	Now draw the wires between blocks
 */
sam::SAManagement(fake);
crypto::Crypto(sam);

 ethin :: InputEth(fake, $MyETH);
 output :: Output($MyETH);
 arpquerier :: ARPQuerier(fake, $MyETH);
 system :: System(fake);
 routingtable :: CastorRoutingTable;
 history :: CastorHistory(crypto);
 handlepkt :: CastorHandlePKT(fake, routingtable, history, crypto);
 handleack :: CastorHandleACK(fake, routingtable, history, crypto);
 flowDB :: CastorFlowStub;
 flow_merkle :: CastorFlowMerkle(flowDB, crypto);
 //source :: CastorSource(flowDB);
 castorclassifier :: CastorClassifier;


handleIPPacket :: HandleIPPacket(flowDB, crypto, fake);
 

 



 ethin[1] 		-> output;			// PUSH new ARP Responses back to device
 ethin[0] 		-> [1]arpquerier;		// Push incoming arp responses to querer
 ethin[2] 		-> Strip(14)
 				-> castorclassifier; 	// Classify received packets			
 

 arpquerier 	-> output;				// Send ethernet encapsulated packets to output
 
 system		-> handleIPPacket;
 handleIPPacket -> handlepkt;			// Process new generated packets
 
 castorclassifier[0]	-> handlepkt;	// Process Packets
 castorclassifier[1]	-> handleack;   // Process ACKs
 castorclassifier[2] 	-> ToHost(fake0);// Discard non Castor

 handlepkt[0]		-> system;
 handlepkt[1]		-> arpquerier;			// Pack packets into Ethernet and transmit them;				
 handlepkt[2] 		-> arpquerier;			// Send back ACKs
 handleack 		 	-> arpquerier;

	
