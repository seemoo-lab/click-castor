elementclass CastorLocalXcastPkt {
	$myIP, $history, $crypto |

	input
		-> CastorXcastAnnotateAckAuth($crypto)
		-> validateAtDest :: CastorValidateFlowAtDestination($crypto)
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
	validateAtDest[1]
		-> CastorPrint("Packet authentication failed", $myIP)
		-> null;

}

elementclass CastorForwardXcastPkt {
	$myIP, $routingtable, $history, $promisc |

	input
		-> CastorXcastLookupRoute($routingtable)		// Lookup the route for the packet
		-> CastorAddXcastPktToHistory($history)
		-> CastorTimeout($routingtable, $history, $timeout, $myIP, false)
		//-> CastorPrint('Forwarding', $myIP)
		-> rec :: CastorRecordPkt
		-> IPEncap($CASTORTYPE, $myIP, DST_ANNO)	// Encapsulate in a new IP Packet
		-> CastorXcastResetDstAnno($promisc) // We want to unicast if possible
		-> output;
}

/**
 * Input:	Castor PKT
 * Output(0):	PKT for local host
 * Output(1):	New ACK
 * Output(2):	Forwarded PKT
 */
elementclass CastorHandleXcastPkt{
	$myIP, $routingtable, $history, $crypto, $promisc |

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
		-> sendAck :: CastorSendAck($myIP)
		-> [1]output;
	
	// Need to retransmit ACK
	checkDuplicate[1]
		-> CastorRetransmitAck($history, $myIP)
		-> sendAck
		-> [1]output;
	
	// PKT needs to be forwarded
	destinationClassifier[1]
		-> forward :: CastorForwardXcastPkt($myIP, $routingtable, $history, $promisc)
		-> [2]output;

	// If invalid or duplicate -> discard
	null :: Discard;
	forwarderClassifier[1]
		//-> CastorPrint("Node not in forwarder list", $myIP)
		-> null;
	checkDuplicate[2]
		//-> CastorPrint("Duplicate", $myIP)
		-> CastorAddXcastPktToHistory($history) // Add sender to history
		-> null;
	validate[1]
		-> CastorPrint("Flow authentication failed", $myIP)
		-> null;

}