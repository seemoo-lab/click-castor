define(
	$fullSend false,
);

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
		-> HostEtherFilter($myAddr) // FromSimDevice is always in promisc mode, so filter here
		-> BroadcastPainter
		-> annoDstAddr :: GetEtherAddress(ANNO 6, OFFSET dst)
		-> ethclassifier :: Classifier(12/88B6, 12/88B5, -) // (0) Castor PKT/ACK; (1) beacon; (2) other
		-> removeEthernetHeader :: Strip(14)
		-> cclassifier :: Classifier(0/c?, 0/a?, -);

	cclassifier[0] // Castor PKTs -> output 0
		-> [0]output;

	cclassifier[1] // Castor ACKs -> output 1
		-> [1]output;

	cclassifier[2] // Malformed Castor packet
		-> Discard;

	ethclassifier[1] // Beacon
		-> Discard

	ethclassifier[2] // Other packet
		-> [2]output;
}

elementclass DynamicEtherEncap {
	$myAddr |

	input
		-> EtherEncap($ETHERTYPE_CASTOR, $myAddr, 00:00:00:00:00:00)
		-> StoreEtherAddress(OFFSET dst, ANNO 6)
		-> output;
}

elementclass CastorHandleAck {
	$myIP, $routingtable, $timeouttable, $ratelimits, $history, $neighbors, $crypto |

	// Regular ACK flow
	input
		-> calcPid :: CastorAnnotatePid($crypto)
		-> authenticate :: CastorAuthenticateAck($history, $CASTOR_VERSION)
		-> updateTimeout :: CastorUpdateTimeout($timeouttable, $history, VERBOSE false)
		-> updateEstimates :: CastorUpdateEstimates($routingtable, $history)
		-> CastorAddAckToHistory($history)
		//-> CastorPrint('Received valid', $myIP)
		-> noLoopback :: CastorNoLoopback($history, $myIP)
		-> updateRate :: CastorUpdateRateLimit($ratelimits, $history)
		-> CastorSetAckNexthop($history, $neighbors)
		-> recAck :: CastorRecordPkt
		-> output;

	// Discarding...
	null :: Discard;
	authenticate[1]
		//-> CastorPrint("Unknown corresponding PKT", $myIP)
		-> null;
	authenticate[2]
		-> CastorPrint("Too late (SHOULD NO LONGER OCCUR!)", $myIP)
		-> null;
	authenticate[3]
		// Might rarely happen if MAC ACK was lost and Castor ACK is retransmitted
		-> CastorPrint("ACK duplicate from same neighbor", $myIP)
		-> null;
	authenticate[4]
		//-> CastorPrint("ACK from different neighbor than PKT was forwarded to", $myIP)
		-> null;
	authenticate[5]
		-> CastorPrint("ACK from same neighbor as initial PKT sender", $myIP)
		//-> CastorAddAckToHistory($history)
		-> null;
	updateEstimates[1]
		//-> CastorPrint("Duplicate, add to history", $myIP)
		-> CastorAddAckToHistory($history)
		-> null;
	noLoopback[1]
		//-> CastorPrint("Don't send to myself", $myIP)
		-> null;
}

elementclass CastorBlackhole {
	input
		-> filter :: CastorUnicastFilter
		-> output;
		
	filter[1]
		-> rec :: CastorRecordPkt
		-> Discard;
}
