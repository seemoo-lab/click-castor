/*******************
 * Wire the Blocks *
 *******************/

fromhost
	-> handleIpPacket
	-> annoSrcAddr :: SetEtherAddress(ANNO 0, ADDR fake)
	-> handlepkt;		// Process new generated packets

castorclassifier[0]
	-> ratelimiter // Rate limit PKTs
	-> handlepkt;  // Process PKTs
castorclassifier[1] -> handleack; // Process ACKs
castorclassifier[2] -> [1]tohost; // Deliver non-Castor packets directly to host

handlepkt[0] -> removeHeader -> [0]tohost; // Deliver PKT to host
handlepkt[1] -> DynamicEtherEncap(fake, neighbors, crypto) -> [1]ethout; // Return ACK
handlepkt[2] -> DynamicEtherEncap(fake, neighbors, crypto) -> [0]ethout; // Forward PKT
handleack    -> DynamicEtherEncap(fake, neighbors, crypto) -> [1]ethout; // Forward ACK

replayratelimits :: CastorRateLimitTable(MIN $replayRateMax, MAX $replayRateMax, INIT $replayRateMax);
replaystore[0,1]
	=> ([0] -> CastorRateLimiter(true, replayratelimits, $replayRateMax) -> CastorAddFlowAuthenticator(flowtable, true) -> output;
		[1] -> CastorRateLimiter(true, replayratelimits, $replayRateMax) -> output;)
	-> DynamicEtherEncap(fake, neighbors, crypto)
	-> [1]ethout;

beacons :: NeighborBeaconGenerator($beaconingInterval, fake, $neighborsEnable)
	-> [2]ethout;

ratelimiter[1]
	-> CastorPrint("Rate limiter drop", fake)
	-> recDrop :: CastorRecordPkt
	-> Discard;
