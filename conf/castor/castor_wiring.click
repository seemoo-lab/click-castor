/*******************
 * Wire the Blocks *
 *******************/

fromhost
	-> handleIpPacket
	-> annoSrcAddr :: SetEtherAddress(ANNO 0, ADDR fake)
	-> handlepkt;		// Process new generated packets

debug_handler
	//-> createDebugPkt
	-> annoSrcAddr;

castorclassifier[0]
	-> ratelimiter // Rate limit PKTs
	-> handlepkt;  // Process PKTs
castorclassifier[1] -> handleack; // Process ACKs

handlepkt[0] -> removeHeader -> [0]tohost; // Deliver PKT to host
handlepkt[1] -> DynamicEtherEncap(fake, neighbors, crypto) -> [1]ethout; // Return ACK
handlepkt[2] -> DynamicEtherEncap(fake, neighbors, crypto) -> [0]ethout; // Forward PKT
handleack[0] -> DynamicEtherEncap(fake, neighbors, crypto) -> [1]ethout; // Forward ACK
handleack[1] -> debug_handler;

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
