/**
 * Paint frames that were broadcasted by the sender
 */
elementclass BroadcastPainter {
	input[0]
		-> broadcastFilter :: HostEtherFilter(FF:FF:FF:FF:FF:FF)
		-> Paint(10, ANNO 38) // Packet was broadcast to us
		-> output;

	broadcastFilter[1]
		-> output; // Packet was unicasted to us (no paint)
}

elementclass CastorClassifier {
	$myAddr, $neighbors |

	input
		-> annoSrcAddr :: GetEtherAddress(ANNO 0, OFFSET src)
		-> AddNeighbor($neighbors, ENABLE $neighborsEnable) // add all neighbors that we receive transmissions from
		-> BroadcastPainter
		-> ethclassifier :: Classifier(12/88B6, 12/88B5, -) // (0) Castor PKT/ACK; (1) beacon; (2) other
		-> removeEthernetHeader :: Strip(14)
		-> forwarderFilter :: ForwarderFilter($myAddr)
		-> NeighborAuthCheckICV(crypto, $neighborsEnable)
		=> ( [0] -> NeighborAuthStripICV -> output;
		     [1] /*-> CastorPrint("No valid neighbor-to-neighbor ICV", fake)*/ -> Discard; )
		-> removeForwarderList :: RemoveForwarderList
		-> cclassifier :: Classifier(0/c?, 0/a?, -);

	cclassifier[0] // Castor PKTs -> output 0
		-> [0]output;

	cclassifier[1] // Castor ACKs -> output 1
		-> [1]output;

	cclassifier[2] // Malformed Castor packet
		-> Print("[DROP] malformed Castor packet")
		-> Discard;

	ethclassifier[1] // Beacon
		// Already updated Neighbor table
		-> Discard

	ethclassifier[2] // other
		//-> Print("[DROP] non-Castor packet")
		-> Discard;
}

elementclass DynamicEtherEncap {
	$myAddr, $neighbors, $crypto |

	input
		-> AddForwarderList
		-> NeighborAuthAddICV($neighbors, $crypto, true)
		=> ( [0] -> output;
		     [1] /* -> CastorPrint("No neighbors, no ICV added", fake) */ -> output; )
		-> EtherEncap($ETHERTYPE_CASTOR, $myAddr, 00:00:00:00:00:00)
		-> StoreEtherAddress(OFFSET dst, ANNO 12)
		-> output;
}

elementclass CastorHandleAck {
	$myIP, $routingtable, $flowtable, $timeouttable, $ratelimits, $history, $neighbors, $crypto |

	// Regular ACK flow
	input
		-> isDbg :: CastorIsDbg
		-> calcPid :: CastorAnnotatePid($crypto)
		-> AddReplayAck(replaystore)
		-> authenticate :: CastorAuthenticateAck($history, $CASTOR_VERSION)
		-> updateTimeout :: CastorUpdateTimeout($timeouttable, $history, VERBOSE false)
		-> updateEstimates :: CastorUpdateEstimates($routingtable, $history, $copyEstimators)
		-> CastorAddAckToHistory($history, $flowtable)
		//-> CastorPrint('Received valid', $myIP)
		-> noLoopback :: CastorNoLoopback($history, $myIP)
		// It does not make sense to update the rate limit packets that I sent myself
		-> updateRate :: CastorUpdateRateLimit($ratelimitEnable, $ratelimits, $history)
		-> setAckNexthop :: CastorSetAckNexthop($history, $neighbors)
		-> recAck :: CastorRecordPkt
		-> output;

	isDbg[1] 
		-> CastorAnnotateDebugPid
		-> debugAuthenticateAck :: CastorDebugAuthenticateAck($history, $myIP)
		-> isInsp :: CastorIsInsp
		-> debugNoLoopback :: CastorNoLoopback($history, $myIP)
		-> setAckNexthop;

	isInsp[1] 
		-> insertPath :: CastorInsertPath($myIP, $myIP)
		-> debugNoLoopback;

	// Discarding...
	null :: Discard;
	authenticate[1]
		//-> CastorPrint("Unknown corresponding PKT", $myIP)
		-> null;
	authenticate[2]
		// Might rarely happen if MAC ACK was lost and Castor ACK is retransmitted
		//-> CastorPrint("ACK duplicate from same neighbor", $myIP)
		-> null;
	authenticate[3]
		//-> CastorPrint("ACK from different neighbor than PKT was forwarded to", $myIP)
		-> null;
	authenticate[4]
		//-> CastorPrint("ACK from same neighbor as initial PKT sender", $myIP)
		-> null;
	updateEstimates[1]
		//-> CastorPrint("Duplicate, add to history", $myIP)
		-> CastorAddAckToHistory($history, $flowtable)
		-> null;
	noLoopback[1]
		//-> CastorPrint("Don't send to myself", $myIP)
		-> null;

	debugNoLoopback[1]
		-> [1]output;
	
	debugAuthenticateAck[1] -> CastorDebugPrint("Couldn't authenticate ACK", $myIP) -> null;
}

elementclass CastorBlackhole {
	input
		-> filter :: CastorUnicastFilter
		-> output;

	filter[1]
		-> rec :: CastorRecordPkt
		-> Discard;
}
