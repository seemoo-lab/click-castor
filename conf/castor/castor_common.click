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
 * Appends Castor header to IP (unicast) packet
 */
elementclass CastorHandleUnicastIpPacket {
	$myIP, $flowDB, $crypto |

	-> CastorAddHeader($flowDB)
	-> CastorEncryptACKAuth($crypto)
	//-> CastorPrint('Send', $myIP, $fullSend)
	-> rec :: CastorRecordPkt
	-> output;
}

/**
 * Appends Castor Xcast header to IP (multicast) packet
 */
elementclass CastorHandleMulticastIpPacket {
	$myIP, $flowDB, $crypto |

	map :: CastorXcastDestinationMap

	input
	-> CastorXcastSetFixedHeader($flowDB)
	-> CastorXcastSetDestinations($crypto, map)
	//-> CastorPrint('Send', $myIP, $fullSend)
	-> rec :: CastorRecordPkt
	-> output;
}

elementclass CastorHandleMulticastToUnicastIpPacket {
	$myIP, $flowDB, $crypto |
	
	map :: CastorXcastDestinationMap

	input
	-> CastorXcastToUnicast(map)
	=> (input[0] -> output;
	    input[1] -> SetIPChecksum -> output;)
	-> CastorAddHeader($flowDB)
	-> CastorEncryptACKAuth($crypto)
	//-> CastorPrint('Send', $myIP, $fullSend)
	-> rec :: CastorRecordPkt
	-> output;
}

elementclass CastorClassifier {

	$myIP |
	
	input
		-> CheckIPHeader
		-> annotateSourceAddress :: GetIPAddress(IP src, ANNO 4) // Put source address after dst_ip_anno()
		-> annotateDestAddress :: GetIPAddress(IP dst, ANNO 0)
		-> addressfilter :: IPClassifier(dst host $myIP or 255.255.255.255, -) // We filter by IP address
		-> ipclassifier :: IPClassifier(ip proto $CASTORTYPE, -)
		-> StripIPHeader
		-> cclassifier :: Classifier(0/c?, 0/a?);

	addressfilter[1]
		-> Discard; // not intended for us

	cclassifier[0] // Castor PKTs -> output 0
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
	$myIP, $routingtable, $history, $neighbors, $crypto, $promisc |

	// Regular ACK flow
	input
		-> calcPid :: CastorAnnotatePid($crypto)
		-> authenticate :: CastorAuthenticateAck($crypto, $history, $CASTOR_VERSION)
		-> updateEstimates :: CastorUpdateEstimates($crypto, $routingtable, $history)
		-> CastorAddAckToHistory($crypto, $history)
		//-> CastorPrint('Received valid', $myIP)
		-> noLoopback :: CastorNoLoopback($history, $myIP)
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
		//-> CastorPrint("Duplicate from same neighbor", $myIP)
		-> null;
	authenticate[4]
		//-> CastorPrint("ACK from different neighbor than PKT was forwarded to", $myIP)
		-> null;
	authenticate[5]
		-> CastorPrint("ACK from same neighbor as initial PKT sender", $myIP)
		//-> CastorAddAckToHistory($crypto, $history)
		-> null;
	updateEstimates[1]
		//-> CastorPrint("Duplicate, add to history", $myIP)
		-> CastorAddAckToHistory($crypto, $history)
		-> null;
	noLoopback[1]
		//-> CastorPrint("Don't send to myself", $myIP)
		-> null;
}

elementclass CastorBlackhole {
	$myIP |
	
	input
		-> filter :: CastorUnicastFilter($myIP)
		-> output;
		
	filter[1]
		-> rec :: CastorRecordPkt
		-> Discard;
}
