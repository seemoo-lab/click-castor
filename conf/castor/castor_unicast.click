elementclass CastorLocalPkt {
	$myIP, $history, $crypto |

	input
		-> CastorPrint('Packet arrived at destination', $myIP)
		-> CastorDecryptACKAuth($crypto)
		-> validateAtDest :: CastorValidateFlowAtDestination($crypto)
		-> CastorAddPKTToHistory($history)
		-> rec :: CastorRecordPkt
		-> genAck :: CastorCreateAck
		-> [0]output;

	genAck[1] // Generate ACK for received PKT
		-> CastorPrint('Generated', $myIP)
		-> CastorAddAckToHistory($crypto, $history)
		-> IPEncap($CASTORTYPE, $myIP, 255.255.255.255)
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
		//-> CastorPrint('Forwarding Packet', $myIP)
		-> CastorLookupRoute($routingtable)		// Lookup the route for the packet
		-> CastorAddPKTToHistory($history)
		-> CastorTimeout($routingtable, $history, $timeout, $myIP)
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
		-> [1]output;
	
	// PKT needs to be forwarded
	destinationClassifier[1]
		-> forward :: CastorForwardPkt($myIP, $routingtable, $history)
		-> [2]output;

	// If invalid or duplicate -> discard
	null :: Discard;
	checkDuplicate[1]
		//-> CastorPrint("Duplicate", $myIP)
		-> null;
	validate[1]
		-> CastorPrint("Packet authentication failed", $myIP)
		-> null;

}