/**
 * Appends Castor Xcast header to IP (multicast) packet
 */
elementclass CastorHandleMulticastIpPacket {
	$myIP, $flowDB, $crypto |

	map :: CastorXcastDestinationMap

	input
	-> CastorXcastSetFixedHeader($flowDB)
	-> CastorXcastSetDestinations($crypto, map)
	//-> CastorPrint('Send', $myIP, $fullSend)
	-> rec :: CastorRecordPkt
	-> output;
}

elementclass CastorLocalXcastPkt {
	$myIP, $history, $crypto |

	input
		-> CastorXcastAnnotateAckAuth($crypto)
		-> authPkt :: CastorAuthenticatePkt($crypto)
		-> rec :: CastorRecordPkt
		//-> CastorPrint('Arrived at destination', $myIP)
		-> CastorAddXcastPktToHistory($history)
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

elementclass CastorForwardXcastPkt {
	$myIP, $routeselector, $routingtable, $timeouttable, $history |

	input
		-> route :: CastorXcastLookupRoute($routeselector)
		-> CastorAddXcastPktToHistory($history)
		-> CastorStartTimer($routingtable, $timeouttable, $history, NodeId $myIP, VERBOSE false)
		//-> CastorPrint('Forwarding', $myIP)
		-> rec :: CastorRecordPkt
		-> IPEncap($CASTORTYPE, $myIP, DST_ANNO) // Encapsulate in a new IP Packet
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
elementclass CastorHandleXcastPkt {
	$myIP, $routeselector, $routingtable, $timeouttable, $history, $crypto |

	input
		-> blackhole :: CastorBlackhole // inactive by default
		-> forwarderClassifier :: CastorXcastForwarderClassifier($myIP)
		-> checkDuplicate :: CastorXcastCheckDuplicate($history, $myIP)
		-> validate :: CastorXcastAuthenticateFlow($crypto)
		-> destinationClassifier :: CastorXcastDestClassifier($myIP);

 	// PKT arrived at destination
	destinationClassifier[0]
		-> handleLocal :: CastorLocalXcastPkt($myIP, $history, $crypto)
		-> [0]output;

	sendAck :: CastorSendAck($myIP)
		-> [1]output;

	handleLocal[1]
		-> sendAck;

	// Need to retransmit ACK
	checkDuplicate[1]
		-> CastorAddXcastPktToHistory($history)
		-> CastorXcastRetransmitAck($history, $myIP)
		-> noLoopback :: CastorNoLoopback($history, $myIP) // The src node should not retransmit ACKs
		-> sendAck;

	// PKT needs to be forwarded
	destinationClassifier[1]
		-> forward :: CastorForwardXcastPkt($myIP, $routeselector, $routingtable, $timeouttable, $history)
		-> [2]output;

	// If invalid or duplicate -> discard
	null :: Discard;
	forwarderClassifier[1]
		//-> CastorPrint("Node not in forwarder list", $myIP)
		-> null;
	checkDuplicate[2]
		//-> CastorPrint("Duplicate PKT from different neighbor", $myIP)
		-> CastorAddXcastPktToHistory($history) // Add sender to history
		-> null;
	checkDuplicate[3]
		//-> CastorPrint("Duplicate PKT from same neighbor", $myIP)
		-> null;
	validate[1]
		-> CastorPrint("Flow authentication failed", $myIP)
		-> null;
	noLoopback[1]
		//-> CastorPrint("Trying to retransmit ACK to myself", $myIP)
		-> null;

}
