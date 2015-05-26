elementclass CastorLocalPkt {
	$myIP, $history, $crypto |

	input
		//-> CastorPrint('Packet arrived at destination', $myIP)
		-> CastorDecryptACKAuth($crypto)
		-> authPkt :: CastorAuthenticatePkt($crypto)
		-> CastorAddPKTToHistory($history)
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
	$myIP, $routeselector, $routingtable, $history |

	input
		-> route :: CastorLookupRoute($routeselector)
		-> CastorAddPKTToHistory($history)
		-> CastorTimeout($routingtable, $history, $timeout, NodeId $myIP, VERBOSE false)
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
	$myIP, $routeselector, $routingtable, $history, $crypto |

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
		-> forward :: CastorForwardPkt($myIP, $routeselector, $routingtable, $history)
		-> [2]output;

	handleLocal[1]
		-> sendAck :: CastorSendAck($myIP)
		-> [1]output;
	
	// Need to retransmit ACK
	checkDuplicate[1]
		//-> CastorPrint("Duplicate pid, retransmit ACK", $myIP)
		-> CastorAddPKTToHistory($history)
		-> CastorRetransmitAck($history, $myIP)
		-> noLoopback :: CastorNoLoopback($history, $myIP) // The src node should not retransmit ACKs
		-> sendAck;

	// If invalid or duplicate -> discard
	null :: Discard;
	checkDuplicate[2]
		//-> CastorPrint("Duplicate PKT from different neighbor", $myIP)
		-> CastorAddPKTToHistory($history) // Add sender to history
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
