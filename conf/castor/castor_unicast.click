elementclass CastorLocalPkt {
	$myIP, $history, $crypto |

	input
		//-> CastorPrint('Packet arrived at destination', $myIP)
		-> CastorDecryptACKAuth($crypto)
		-> validateAtDest :: CastorValidateFlowAtDestination($crypto)
		-> CastorAddPKTToHistory($history)
		-> rec :: CastorRecordPkt
		-> genAck :: CastorCreateAck
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

elementclass CastorForwardPkt {
	$myIP, $routingtable, $history |

	input
		-> CastorLookupRoute($routingtable)		// Lookup the route for the packet
		-> CastorAddPKTToHistory($history)
		-> CastorTimeout($routingtable, $history, $timeout, $myIP, false)
		//-> CastorPrint('Forwarding Packet', $myIP)
		-> rec :: CastorRecordPkt
		-> IPEncap($CASTORTYPE, $myIP, DST_ANNO)	// Encapsulate in a new IP Packet
		-> output;

}

/**
 * Input:	Castor PKT
 * Output(0):	PKT for local host
 * Output(1):	New ACK
 * Output(2):	Forwarded PKT
 */
elementclass CastorHandlePkt {
	$myIP, $routingtable, $history, $crypto |

	input
		-> checkDuplicate :: CastorCheckDuplicate($history)
		-> validate :: CastorValidateFlow($crypto)
		-> destinationClassifier :: CastorDestClassifier($myIP);

 	// PKT arrived at destination
	destinationClassifier[0]
		-> handleLocal :: CastorLocalPkt($myIP, $history, $crypto)
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
		-> blackhole :: CastorBlackhole($myIP) // By default inactive
		-> forward :: CastorForwardPkt($myIP, $routingtable, $history)
		-> [2]output;

	// If invalid or duplicate -> discard
	null :: Discard;
	checkDuplicate[2]
		//-> CastorPrint("Duplicate", $myIP)
		//-> CastorAddPKTToHistory($history) // Add sender to history (FIXME: if disabled, protocol performs much better... why?!)
		-> null;
	validate[1]
		-> CastorPrint("Flow authentication failed", $myIP)
		-> null;

}