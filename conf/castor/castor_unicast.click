/**
 * Creates IP unicast from multicast packets
 */
elementclass CastorHandleMulticastToUnicastIpPacket {
	$myAddrInfo, $flowmanager, $flowtable, $crypto, $map |

	input
	-> XcastToUnicast($map)
	=> (input[0] -> output;
		input[1] -> SetIPChecksum -> output;)
	-> CastorAddHeader($flowmanager, $flowtable, $forceNonce)
	-> CastorCalcICV($crypto)
	//-> CastorPrint('Send', $myAddrInfo)
	-> rec :: CastorRecordPkt
	-> output;
}

elementclass CastorLocalPkt {
	$myIP, $flowtable, $timeouttable, $history, $crypto |

	input
		//-> CastorPrint('Packet arrived at destination', $myIP)
		-> CastorStripFlowAuthenticator
		// Check ICV before trying to authenticate flow
		-> auth :: CastorCheckICV($crypto)
		-> CastorIsSync[0,1]
		=> (input[0]
				-> CastorHasCompleteFlow($flowtable)[0,1]
				=> (input[0] -> output;
					input[1] -> CastorPrint("SYN missing", $myIP) -> Discard;)
				-> output;
			// this is only done once per flow
			input[1] -> CastorReconstructFlow($flowtable, $crypto) -> output;)
		-> authFlow :: CastorAuthenticateFlow($flowtable, $crypto)
		-> CastorAddPktToHistory($history)
		-> CastorStartTimer($timeouttable, $history, ID $myIP, VERBOSE false)
		-> rec :: CastorRecordPkt
		-> genAck :: CastorCreateAck($flowtable)
		-> [0]output;

	genAck[1] // Generate ACK for received PKT
		//-> CastorPrint('Generated', $myIP)
		-> calcPid :: CastorAnnotatePid($crypto)
		-> CastorAddAckToHistory($history, $flowtable)
		-> [1]output; // Push ACKs to output 1

	// If invalid -> discard
	null :: Discard;
	authFlow[1]
		-> CastorPrint("Flow authentication failed", $myIP)
		-> null;
	auth[1]
		-> CastorPrint("Invalid ICV", $myIP)
		-> null;
}

elementclass CastorForwardPkt {
	$myIP, $routeselector, $routingtable, $flowtable, $timeouttable, $ratelimits, $history, $crypto |

	input
		-> auth :: CastorAuthenticateFlow($flowtable, $crypto)
		-> AddReplayPkt(replaystore)
		-> blackhole :: CastorBlackhole // inactive by default
		-> route :: CastorLookupRoute($routeselector)
		-> CastorAddPktToHistory($history)
		-> CastorStartTimer($timeouttable, $history, $routingtable, $ratelimits, ID $myIP, VERBOSE false)
		-> addFlowAuthenticator :: CastorAddFlowAuthenticator($flowtable, $fullFlowAuth)
		-> CastorSetARQ(false)
		//-> CastorPrint('Forwarding Packet', $myIP)
		-> rec :: CastorRecordPkt
		-> output;

	// Bounce back to sender
	auth[2]
		-> CastorPrint("Flow authentication failed (insufficient flow auth elements)", $myIP)
		-> CastorMirror($myIP)
		-> CastorSetARQ(true)
		-> output;

	null :: Discard;
	auth[1]
		-> CastorPrint("Flow authentication failed", $myIP)
		-> null;

	route[1]
		-> CastorPrint("No suitable PKT forwarding contact", $myIP)
		-> null;
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
		-> isARQ :: CastorIsARQ
		-> checkDuplicate :: CastorCheckDuplicate($history, $flowtable, $replayProtect)
		-> destinationClassifier :: CastorDestClassifier($myIP);

 	// PKT arrived at destination
	destinationClassifier[0]
		-> handleLocal :: CastorLocalPkt($myIP, $flowtable, $timeouttable, $history, $crypto)
		-> [0]output;

	// PKT needs to be forwarded
	destinationClassifier[1]
		-> forward :: CastorForwardPkt($myIP, $routeselector, $routingtable, $flowtable, $timeouttable, $ratelimits, $history, $crypto)
		-> [2]output;

	isARQ[1] -> CastorPrint("Retransmit", $myIP) -> destinationClassifier;

	handleLocal[1]
		-> recAck :: CastorRecordPkt
		-> [1]output;

	// Need to retransmit ACK
	checkDuplicate[1]
		-> CastorAddPktToHistory($history)
		-> CastorRetransmitAck($history)
		-> noLoopback :: CastorNoLoopback($history, $myIP) // The src node should not retransmit ACKs
		//-> CastorPrint("Duplicate pid, retransmit ACK", $myIP)
		-> recAck;

	// If invalid or duplicate -> discard
	null :: Discard;
	checkDuplicate[2]
		//-> CastorPrint("PKT duplicate, but from different neighbor", $myIP)
		-> CastorAddPktToHistory($history) // Add sender to history
		-> null;

	checkDuplicate[3]
		// Might rarely happen if MAC ACK was lost and Castor PKT is retransmitted
		//-> CastorPrint("PKT duplicate from same neighbor", $myIP)
		-> null;

	noLoopback[1]
		//-> CastorPrint("Source trying to retransmit", $myIP)
		-> null;

}
