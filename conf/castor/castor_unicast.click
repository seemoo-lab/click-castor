/**
 * Creates IP unicast from multicast packets
 */
elementclass CastorHandleMulticastToUnicastIpPacket {
	$myAddrInfo, $flowmanager, $crypto, $map |
	
	input
	-> CastorXcastToUnicast($map)
	=> (input[0] -> output;
	    input[1] -> SetIPChecksum -> output;)
	-> CastorAddHeader($flowmanager)
	-> CastorEncryptAckAuth($crypto)
	-> CastorCalcICV($crypto)
	//-> CastorPrint('Send', $myAddrInfo, $fullSend)
	-> rec :: CastorRecordPkt
	-> output;
}

elementclass CastorLocalPkt {
	$myIP, $timeouttable, $history, $crypto |

	input
		//-> CastorPrint('Packet arrived at destination', $myIP)
		-> CastorStripFlowAuthenticator
		-> auth :: CastorCheckICV($crypto)
		-> CastorDecryptAckAuth($crypto)
		-> authPkt :: CastorAuthenticatePkt($crypto)
		-> CastorAddPktToHistory($history)
		-> CastorStartTimer($timeouttable, $history, ID $myIP, VERBOSE false)
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
	auth[1]
		-> CastorPrint("Invalid ICV", $myIP)
		-> null;
	authPkt[1]
		-> CastorPrint("Packet authentication failed", $myIP)
		-> null;
}

elementclass CastorForwardPkt {
	$myIP, $routeselector, $routingtable, $timeouttable, $ratelimits, $history, $crypto |

	input
		-> route :: CastorLookupRoute($routeselector)
		-> CastorAddPktToHistory($history)
		-> CastorStartTimer($timeouttable, $history, $routingtable, $ratelimits, ID $myIP, VERBOSE false)
		//-> CastorPrint('Forwarding Packet', $myIP)
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
elementclass CastorHandlePkt {
	$myIP, $routeselector, $routingtable, $flowtable, $timeouttable, $ratelimits, $history, $crypto |

	input
		-> blackhole :: CastorBlackhole // inactive by default
		-> checkDuplicate :: CastorCheckDuplicate($history)
		-> authenticate :: CastorAuthenticateFlow($flowtable, $crypto)
		-> destinationClassifier :: CastorDestClassifier($myIP);

	checkDuplicate[4] -> CastorPrint("Retransmit", $myIP) -> authenticate;

 	// PKT arrived at destination
	destinationClassifier[0]
		-> handleLocal :: CastorLocalPkt($myIP, $timeouttable, $history, $crypto)
		-> [0]output;
	
	// PKT needs to be forwarded
	destinationClassifier[1]
		-> forward :: CastorForwardPkt($myIP, $routeselector, $routingtable, $timeouttable, $ratelimits, $history, $crypto)
		-> addFlowAuthenticator :: CastorAddFlowAuthenticator($flowtable, $fullFlowAuth)
		-> [2]output;

	handleLocal[1]
		-> recAck :: CastorRecordPkt
		-> [1]output;
	
	// Need to retransmit ACK
	checkDuplicate[1]
		//-> CastorPrint("Duplicate pid, retransmit ACK", $myIP)
		-> CastorAddPktToHistory($history)
		-> CastorRetransmitAck($history)
		-> noLoopback :: CastorNoLoopback($history, $myIP) // The src node should not retransmit ACKs
		-> recAck;

	// Bounce back to sender
	authenticate[2]
		-> CastorPrint("Flow authentication failed (insufficient flow auth elements)", $myIP)
		-> CastorMirror($myIP)
		-> [2]output;

	// If invalid or duplicate -> discard
	null :: Discard;
	checkDuplicate[2]
		//-> CastorPrint("PKT duplicate, but from different neighbor", $myIP)
		-> CastorAddPktToHistory($history) // Add sender to history
		-> null;

	checkDuplicate[3]
		// Might rarely happen if MAC ACK was lost and Castor PKT is retransmitted
		-> CastorPrint("PKT duplicate from same neighbor", $myIP)
		-> null;

	authenticate[1]
		-> CastorPrint("Flow authentication failed", $myIP)
		-> null;

	noLoopback[1]
		//-> CastorPrint("Trying to retransmit ACK to myself", $myIP)
		-> null;

}
