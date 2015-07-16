require(
	library castor_settings.click,
	library castor_io.click,
);

elementclass RecordPkt {
	$map |
	
	input
		-> rec :: FloodingRecordPkt($map)
		-> output;
}

elementclass FloodingBlackhole {
	$map |
	
	input
		-> filter :: FloodingBlackholeFilter
		-> output;
		
	filter[1]
		-> rec :: FloodingRecordPkt($map)
		-> Discard;

}

elementclass HandleIPPacket {
	$map |
	
	input
		-> FloodingSetPktId
		-> rec :: FloodingRecordPkt($map)
		-> output
}

elementclass FloodingHandlePkt {
	$myIP, $map |

	input
		-> CheckIPHeader
		-> blackhole :: FloodingBlackhole($map)
		-> duplicateClassifier :: FloodingCheckDuplicate
		-> destinationClassifier :: FloodingDestinationClassifier($myIP, $map)
		-> handleLocal :: RecordPkt($map)
		-> [0]output;
		
	destinationClassifier[1]
		-> forward :: RecordPkt($map)
		-> [1]output; // Forward the message
		
	duplicateClassifier[1]
		-> Discard;

	// Dummy element for evaluation in ns-3
	Idle() -> sendAck :: { -> recAck :: FloodingRecordPkt(map) -> } -> Discard;
}



/*************************
 * Initialize the Blocks *
 *************************/

fromextdev -> ethin :: InputEthNoHostFilter($EthDev, fake);
ethout :: OutputEth($broadcastJitter) -> toextdev;
fromhostdev -> fromhost :: FromHost(fake);
tohost :: ToHost() -> tohostdev;
Idle() -> [1]tohost;

map :: CastorXcastDestinationMap;

handleIpPacket :: HandleIPPacket(map);
handlepkt :: FloodingHandlePkt(fake, map);

/*******************
 * Wire the Blocks *
 *******************/

ethin[1] -> [4]ethout; // Push new ARP Responses to device
ethin[0] -> [3]ethout; // Push incoming ARP responses to querier
Idle()   -> [2]ethout; // Usually used for beacons, but don't use them here
Idle()	 -> [1]ethout; // Usually used for ACKs, but don't use them here
ethin[2]
	-> removeEthernetHeader :: Strip(14)
 	-> handlepkt; // received packets

fromhost -> handleIpPacket -> handlepkt;	// Process new generated packets
 
handlepkt[0]		-> tohost;		// Deliver PKT to host
handlepkt[1]
	-> Paint(10, ANNO 32) // These are always broadcast
	-> [0]ethout; // Forward PKT

// Dummy elements for evaluation in ns-3
neighbors :: Neighbors(0, false);
Idle() -> handleack :: { -> sendAck :: { -> recAck :: FloodingRecordPkt(map) -> } -> } -> Discard;
Idle() -> recBeacon :: CastorRecordPkt -> Discard;
