elementclass CastorLocalXcastPkt {
	$myIP, $history, $crypto |

	input
		-> CastorXcastAnnotateAckAuth($crypto)
		-> authPkt :: CastorAuthenticatePkt($crypto)
		-> rec :: CastorRecordPkt
		//-> CastorPrint('Arrived at destination', $myIP)
		-> CastorAddXcastPktToHistory($history)
		-> genAck :: CastorXcastCreateAck
		-> [0]output;

	genAck[1] // Generate ACK for received PKT
		//-> CastorPrint('Generated', $myIP)
		-> CastorAddAckToHistory($crypto, $history)
		-> [1]output; // Push ACKs to output 1

	// If invalid -> discard
	null :: Discard;
	authPkt[1]
		-> CastorPrint("Packet authentication failed", $myIP)
		-> null;

}

elementclass CastorForwardXcastPkt {
	$myIP, $routeselector, $routingtable, $history, $promisc |

	input
		-> route :: CastorXcastLookupRoute($routeselector)
		-> CastorAddXcastPktToHistory($history)
		-> CastorTimeout($routingtable, $history, $timeout, $myIP, false)
		//-> CastorPrint('Forwarding', $myIP)
		-> rec :: CastorRecordPkt
		-> IPEncap($CASTORTYPE, $myIP, DST_ANNO) // Encapsulate in a new IP Packet
		-> CastorXcastResetDstAnno($promisc) // We want to unicast on the MAC layer if possible
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
	$myIP, $routeselector, $routingtable, $history, $crypto, $promisc |

	input
		-> forwarderClassifier :: CastorXcastForwarderClassifier($myIP)
		-> checkDuplicate :: CastorXcastCheckDuplicate($history, $myIP)
		-> validate :: CastorXcastAuthenticateFlow($crypto)
		-> destinationClassifier :: CastorXcastDestClassifier($myIP);

 	// PKT arrived at destination
	destinationClassifier[0]
		-> handleLocal :: CastorLocalXcastPkt($myIP, $history, $crypto)
		-> [0]output;

	handleLocal[1]
		-> sendAck :: CastorSendAck($myIP)
		-> [1]output;
	
	// Need to retransmit ACK
	checkDuplicate[1]
		-> CastorAddXcastPktToHistory($history)
		-> CastorRetransmitAck($history, $myIP)
		-> noLoopback :: CastorNoLoopback($history, $myIP) // The src node should not retransmit ACKs
		-> sendAck
		-> [1]output;
	
	// PKT needs to be forwarded
	destinationClassifier[1]
		-> blackhole :: CastorBlackhole($myIP) // By default inactive
		-> forward :: CastorForwardXcastPkt($myIP, $routeselector, $routingtable, $history, $promisc)
		-> [2]output;

	// If invalid or duplicate -> discard
	null :: Discard;
	forwarderClassifier[1]
		//-> CastorPrint("Node not in forwarder list", $myIP)
		-> null;
	checkDuplicate[2]
		//-> CastorPrint("Duplicate", $myIP)
		//-> CastorAddXcastPktToHistory($history) // Add sender to history (FIXME: if disabled, protocol performs much better... why?!)
		-> null;
	validate[1]
		-> CastorPrint("Flow authentication failed", $myIP)
		-> null;
	noLoopback[1]
		//-> CastorPrint("Trying to retransmit ACK to myself", $myIP)
		-> null;

}