/**
 * Appends Castor header to IP unicast packet
 */
elementclass CastorHandleUnicastIpPacket {
	$myIP, $flowDB, $crypto |

	-> CastorAddHeader($flowDB)
	-> CastorEncryptAckAuth($crypto)
	//-> CastorPrint('Send', $myIP, $fullSend)
	-> rec :: CastorRecordPkt
	-> output;
}

/**
 * Creates IP unicast from multicast packets
 */
elementclass CastorHandleMulticastToUnicastIpPacket {
	$myIP, $flowDB, $crypto |
	
	map :: CastorXcastDestinationMap

	input
	-> CastorXcastToUnicast(map)
	=> (input[0] -> output;
	    input[1] -> SetIPChecksum -> output;)
	-> CastorAddHeader($flowDB)
	-> CastorEncryptAckAuth($crypto)
	//-> CastorPrint('Send', $myIP, $fullSend)
	-> rec :: CastorRecordPkt
	-> output;
}

elementclass CastorLocalPkt {
	$myIP, $history, $crypto |

	input
		//-> CastorPrint('Packet arrived at destination', $myIP)
		-> CastorDecryptAckAuth($crypto)
		-> authPkt :: CastorAuthenticatePkt($crypto)
		-> CastorAddPktToHistory($history)
		-> rec :: CastorRecordPkt
		-> genAck :: CastorCreateAck
		-> [0]output;

	genAck[1] // Generate ACK for received PKT
		//-> CastorPrint('Generated', $myIP)
		-> calcPid :: CastorAnnotatePid($crypto)
		-> CastorAddAckToHistory($history)
		-> [1]output; // Push ACKs to output 1

	// If invalid -> discard
	null :: Discard;
	authPkt[1]
		-> CastorPrint("Packet authentication failed", $myIP)
		-> null;

}

elementclass CastorForwardPkt {
	$myIP, $routeselector, $routingtable, $timeouttable, $history |

	input
		-> route :: CastorLookupRoute($routeselector)
		-> CastorAddPktToHistory($history)
		-> CastorStartTimer($routingtable, $timeouttable, $history, NodeId $myIP, VERBOSE false)
		//-> CastorPrint('Forwarding Packet', $myIP)
		-> rec :: CastorRecordPkt
		-> IPEncap($CASTORTYPE, $myIP, DST_ANNO)
		-> output;

	route[1]
		-> CastorPrint("No suitable PKT forwarding contact", $myIP)
		-> Discard;

}

/**
 * Input:	Castor PKT
 * Output(0):	PKT for local host
 * Output(1):	New ACK
 * Output(2):	Forwarded PKT
 */
elementclass CastorHandlePkt {
	$myIP, $routeselector, $routingtable, $timeouttable, $history, $crypto |

	input
		-> blackhole :: CastorBlackhole // inactive by default
		-> checkDuplicate :: CastorCheckDuplicate($history)
		-> authenticate :: CastorAuthenticateFlow($crypto)
		-> destinationClassifier :: CastorDestClassifier($myIP);

 	// PKT arrived at destination
	destinationClassifier[0]
		-> handleLocal :: CastorLocalPkt($myIP, $history, $crypto)
		-> [0]output;
	
	// PKT needs to be forwarded
	destinationClassifier[1]
		-> forward :: CastorForwardPkt($myIP, $routeselector, $routingtable, $timeouttable, $history)
		-> [2]output;

	handleLocal[1]
		-> sendAck :: CastorSendAck($myIP)
		-> [1]output;
	
	// Need to retransmit ACK
	checkDuplicate[1]
		//-> CastorPrint("Duplicate pid, retransmit ACK", $myIP)
		-> CastorAddPktToHistory($history)
		-> CastorRetransmitAck($history, $myIP)
		-> noLoopback :: CastorNoLoopback($history, $myIP) // The src node should not retransmit ACKs
		-> sendAck;

	// If invalid or duplicate -> discard
	null :: Discard;
	checkDuplicate[2]
		//-> CastorPrint("Duplicate PKT from different neighbor", $myIP)
		-> CastorAddPktToHistory($history) // Add sender to history
		-> null;

	checkDuplicate[3]
		//-> CastorPrint("Duplicate PKT from same neighbor", $myIP)
		-> null;

	authenticate[1]
		-> CastorPrint("Flow authentication failed", $myIP)
		-> null;

	noLoopback[1]
		//-> CastorPrint("Trying to retransmit ACK to myself", $myIP)
		-> null;

}
