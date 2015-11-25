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
		-> blackhole :: FloodingBlackhole($map)
		-> duplicateFilter :: FloodingCheckDuplicate
		-> destinationClassifier :: FloodingDestinationClassifier($myIP, $map)
		-> handleLocal :: RecordPkt($map)
		-> [0]output;
		
	destinationClassifier[1]
		-> forward :: RecordPkt($map)
		-> [1]output; // Forward the message

	// Dummy element for evaluation in ns-3
	Idle() -> recAck :: FloodingRecordPkt(map) -> Discard;
}



/*************************
 * Initialize the Blocks *
 *************************/

fromextdev -> hostfilter :: HostEtherFilter(fake);
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

Idle()   -> [2]ethout; // Usually used for beacons, but don't use them here
Idle()	 -> [1]ethout; // Usually used for ACKs, but don't use them here
hostfilter -> removeEthernetHeader :: Strip(14) -> CheckIPHeader -> handlepkt; // received packets

fromhost -> handleIpPacket -> handlepkt;	// Process new generated packets
 
handlepkt[0]		-> tohost;		// Deliver PKT to host
handlepkt[1]
	-> EtherEncap(0x0800, fake, ff:ff:ff:ff:ff:ff)
	-> Paint(10, ANNO 38) // These are always broadcast
	-> [0]ethout; // Forward PKT

// Dummy elements for evaluation in ns-3
neighbors :: Neighbors(0, false);
Idle() -> handleack :: { -> recAck :: FloodingRecordPkt(map) -> } -> Discard;
Idle() -> recBeacon :: CastorRecordPkt -> Discard;
