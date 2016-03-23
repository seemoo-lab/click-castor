/*******************
 * Wire the Blocks *
 *******************/

fromhost
	-> handleIpPacket
	-> annoSrcAddr :: SetEtherAddress(ANNO 0, ADDR fake)
	-> handlepkt;		// Process new generated packets

castorclassifier[0]
	-> NeighborAuthCheckICV(crypto)
	=> ( [0] -> NeighborAuthStripICV -> output;
	     [1] -> CastorPrint("Invalid neighbor-to-neighbor ICV", fake) -> Discard; )
	-> ratelimiter // Rate limit PKTs
	-> handlepkt;  // Process PKTs
castorclassifier[1] -> handleack; // Process ACKs
castorclassifier[2] -> [1]tohost; // Deliver non-Castor packets directly to host

handlepkt[0] -> removeHeader -> [0]tohost; // Deliver PKT to host
handlepkt[1] -> NeighborAuthAddICV(crypto) -> DynamicEtherEncap(fake) -> [1]ethout; // Return ACK
handlepkt[2] -> NeighborAuthAddICV(crypto) -> DynamicEtherEncap(fake) -> [0]ethout; // Forward PKT
handleack    -> NeighborAuthAddICV(crypto) -> DynamicEtherEncap(fake) -> [1]ethout; // Forward ACK

replaystore[0,1]
	=> ([0] -> CastorAddFlowAuthenticator(flowtable, true) -> output;
		[1] -> output;)
	-> DynamicEtherEncap(fake)
	-> [1]ethout;

beacons :: NeighborBeaconGenerator($beaconingInterval, fake, $neighborsEnable)
	-> [2]ethout;

ratelimiter[1]
	-> CastorPrint("Rate limiter drop", fake)
	-> recDrop :: CastorRecordPkt
	-> Discard;
