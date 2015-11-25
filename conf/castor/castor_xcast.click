/**
 * Appends Castor Xcast header to IP (multicast) packet
 */
elementclass CastorHandleMulticastIpPacket {
	$myIP, $flowmanager, $crypto, $map |

	input
	-> CastorXcastSetFixedHeader($flowmanager)
	-> CastorXcastSetDestinations($crypto, $map, $myIP)
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
	$myIP, $routeselector, $routingtable, $timeouttable, $ratelimits, $history, $crypto |

	input
		-> route :: CastorXcastLookupRoute($routeselector)
		-> CastorAddXcastPktToHistory($history)
		-> CastorStartTimer($routingtable, $timeouttable, $history, $ratelimits, ID $myIP, VERBOSE true)
		//-> CastorPrint('Forwarding', $myIP)
		-> rec :: CastorRecordPkt
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
	$myIP, $routeselector, $routingtable, $timeouttable, $ratelimits, $history, $crypto |

	input
		-> blackhole :: CastorBlackhole // inactive by default
		-> forwarderClassifier :: CastorXcastForwarderClassifier($myIP)
		-> checkDuplicate :: CastorXcastCheckDuplicate($history, $myIP)
		-> validate :: CastorXcastAuthenticateFlow($crypto)
		-> destinationClassifier :: CastorXcastDestClassifier(ENDID $myIP, ID $myIP);

 	// PKT arrived at destination
	destinationClassifier[0]
		-> handleLocal :: CastorLocalXcastPkt($myIP, $history, $crypto)
		-> [0]output;

	handleLocal[1]
		-> recAck :: CastorRecordPkt
		-> [1]output;

	// Need to retransmit ACK
	checkDuplicate[1]
		-> CastorAddXcastPktToHistory($history)
		-> CastorXcastRetransmitAck($history)
		-> noLoopback :: CastorNoLoopback($history, $myIP) // The src node should not retransmit ACKs
		-> recAck;

	// PKT needs to be forwarded
	destinationClassifier[1]
		-> forward :: CastorForwardXcastPkt($myIP, $routeselector, $routingtable, $timeouttable, $ratelimits, $history, $crypto)
		-> [2]output;

	// If invalid or duplicate -> discard
	null :: Discard;
	forwarderClassifier[1]
		//-> CastorPrint("Node not in forwarder list", $myIP)
		-> null;
	checkDuplicate[2]
		//-> CastorPrint("PKT duplicate, but from different neighbor", $myIP)
		-> CastorAddXcastPktToHistory($history) // Add sender to history
		-> null;
	checkDuplicate[3]
		// Might rarely happen if MAC ACK was lost and Xcastor PKT is retransmitted
		-> CastorPrint("PKT duplicate from same neighbor", $myIP)
		-> null;
	validate[1]
		-> CastorPrint("Flow authentication failed", $myIP)
		-> null;
	noLoopback[1]
		//-> CastorPrint("Trying to retransmit ACK to myself", $myIP)
		-> null;

}
