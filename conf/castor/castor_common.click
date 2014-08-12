define(
	$fullSend false,
);

/**
 * Appends Castor header to IP (unicast) packet
 */
elementclass CastorHandleUnicastIpPacket {
	$myIP, $flowDB, $crypto |

	-> CastorAddHeader($flowDB)
	-> CastorEncryptACKAuth($crypto)
	-> CastorPrint('Send', $myIP, $fullSend)
	-> rec :: CastorRecordPkt
	-> output;
}

/**
 * Appends Castor Xcast header to IP (multicast) packet
 */
elementclass CastorHandleMulticastIpPacket{
	$myIP, $flowDB, $crypto |

	map :: CastorXcastDestinationMap

	input
	-> CastorXcastSetFixedHeader($flowDB)
	-> CastorXcastSetDestinations($crypto, map)
	-> CastorPrint('Send', $myIP, $fullSend)
	-> rec :: CastorRecordPkt
	-> output;
}

elementclass CastorHandleMulticastToUnicastIpPacket {
	$myIP, $flowDB, $crypto |
	
	map :: CastorXcastDestinationMap

	input
	-> CastorXcastToUnicast(map)
	-> CastorAddHeader($flowDB)
	-> CastorEncryptACKAuth($crypto)
	-> CastorPrint('Send', $myIP, $fullSend)
	-> rec :: CastorRecordPkt
	-> output;
}

elementclass CastorClassifier {

	input
		-> MarkIPHeader
		-> CheckIPHeader
		-> ipclassifier :: IPClassifier(ip proto $CASTORTYPE, -)
		-> annotateSenderAddress :: GetIPAddress(12)
		-> StripIPHeader
		-> CastorPrint("Receive", fake)
		-> cclassifier :: Classifier(0/c?, 0/a?);

	cclassifier[0] // Castor PKTs -> output 0
		-> [0]output;

	cclassifier[1] // Castor ACKs -> output 1
		-> [1]output;

	ipclassifier[1] // Other packets -> output 2
		-> [2]output;

}

elementclass CastorHandleAck{
	$myIP, $routingtable, $history, $crypto |

	// Regular ACK flow
	input
		-> calcPid :: CastorAnnotatePid($crypto)
		-> validate :: CastorValidateACK($crypto, $history)
		-> updateEstimates :: CastorUpdateEstimates($crypto, $routingtable, $history)
		-> CastorAddAckToHistory($crypto, $history)
		-> CastorPrint('Received valid', $myIP)
		-> IPEncap($CASTORTYPE, $myIP, 255.255.255.255)
		-> output;

	// Discarding...
	null :: Discard;
	validate[1]
		-> CastorPrint("Unknown corresponding PKT", $myIP)
		-> null;
	validate[2]
		-> CastorPrint("Too late", $myIP)
		-> null;
	validate[3]
		-> CastorPrint("Duplicate from same neighbor", $myIP)
		-> null;
	updateEstimates[1]
		-> CastorPrint("Duplicate", $myIP)
		-> null;
	updateEstimates[2]
		-> CastorPrint("Received from wrong neighbor", $myIP)
		-> null;
}
