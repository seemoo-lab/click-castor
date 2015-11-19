define(
	$fullSend false,
);


elementclass CastorEtherFilter {
	input
		-> beaconClassifier :: Classifier(12/88B5, -) // Castor beacon; other
		-> removeEthernetHeader :: Strip(14)
		-> [1]output; // Castor beacon
		
	beaconClassifier[1]
		-> removeEthernetHeader2 :: Strip(14)
		-> [0]output; // Castor PKT or ACK
}

/**
 * Paint frames that were broadcasted by the sender
 */
elementclass BroadcastPainter {
	input[0]
		-> dstFilter :: IPClassifier(dst host 255.255.255.255, -)
		-> Paint(10, ANNO 32) // Packet was broadcast to us
		-> output;

	dstFilter[1]
		-> output; // Packet was unicasted to us (no paint)
}

elementclass CastorClassifier {

	$myIP, $neighbors, $ratelimits |

	input
		-> CheckIPHeader
		-> AddIPNeighbor($neighbors, ENABLE $neighborsEnable)
		-> BroadcastPainter
		-> annotateSourceAddress :: GetIPAddress(IP src, ANNO 4) // Put source address after dst_ip_anno()
		-> annotateDestAddress :: GetIPAddress(IP dst, ANNO 0)
		-> addressfilter :: IPClassifier(dst host $myIP or 255.255.255.255, -) // We filter by IP address
		-> ipclassifier :: IPClassifier(ip proto $CASTORTYPE, -)
		-> StripIPHeader
		-> cclassifier :: Classifier(0/c?, 0/a?);

	addressfilter[1]
		-> Discard; // not intended for us

	cclassifier[0] // Castor PKTs -> output 0
		-> CastorRateLimiter($ratelimits)
		-> [0]output;

	cclassifier[1] // Castor ACKs -> output 1
		-> [1]output;

	ipclassifier[1] // Other packets -> output 2
		-> [2]output;

}

elementclass CastorSendAck {
	$myIP |
	
	input
		-> recAck :: CastorRecordPkt
		-> IPEncap($CASTORTYPE, $myIP, DST_ANNO)
		-> output;
}

elementclass CastorHandleAck {
	$myIP, $routingtable, $timeouttable, $ratelimits, $history, $neighbors, $crypto, $promisc |

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
		-> CastorSetAckNexthop($history, $neighbors, $promisc)[0,1]
		-> sendAck :: CastorSendAck($myIP)
		-> output;

	// Discarding...
	null :: Discard;
	authenticate[1]
		//-> CastorPrint("Unknown corresponding PKT", $myIP)
		-> null;
	authenticate[2]
		//-> CastorPrint("Too late", $myIP)
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
