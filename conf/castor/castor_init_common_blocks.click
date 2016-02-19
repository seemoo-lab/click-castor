/****************************
 * Initialize common blocks *
 ****************************/

sam::SAManagement(fake);
crypto::Crypto(sam);
flowtable :: CastorFlowTable(crypto);
flowmanager :: CastorFlowManager($flowSize, flowtable, crypto);

groupmap :: CastorXcastDestinationMap;

neighbors :: Neighbors($neighborTimeout, $neighborsEnable);
history :: CastorHistory;
routingtable :: CastorRoutingTable($updateDelta);
timeouttable :: CastorTimeoutTable(INIT $initTo, MIN $minTo, MAX $maxTo);
ratelimits :: CastorRateLimitTable(INIT $initRate, MIN $minRate, MAX $maxRate);

castorclassifier :: CastorClassifier(fake, neighbors)
ratelimiter :: CastorRateLimiter(ratelimits, $bucketSize);

fromextdev -> castorclassifier;
ethout :: OutputEth($broadcastJitter) -> toextdev;
fromhostdev -> fromhost :: FromHost(fake);
tohost :: ToHost() -> tohostdev;
