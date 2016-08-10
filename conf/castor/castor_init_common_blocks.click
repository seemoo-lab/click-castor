/****************************
 * Initialize common blocks *
 ****************************/

sam::SAManagement(fake, fake);
crypto::Crypto(sam);
flowtable :: CastorFlowTable(crypto);
flowmanager :: CastorFlowManager($flowSize, flowtable, crypto);

groupmap :: XcastDestinationMap;

neighbors :: Neighbors($neighborTimeout, $neighborsEnable);
history :: CastorHistory;
routingtable :: CastorRoutingTable($updateDelta);
timeouttable :: CastorTimeoutTable(INIT $initTo, MIN $minTo, MAX $maxTo);
ratelimits :: CastorRateLimitTable(INIT $initRate, MIN $minRate, MAX $maxRate);
//fromhandler :: FromHandler();
//tohandler :: ToHandler();
debug_handler :: CastorDebugHandler(flowmanager);
//createDebugPkt :: CastorCreateDebugPkt(flowmanager, flowtable);

castorclassifier :: CastorClassifier(fake, neighbors)
ratelimiter :: CastorRateLimiter($ratelimitEnable, ratelimits, $bucketSize);

replaystore :: ReplayStore($replayInterval, $replayJitter, $replayCount);

fromextdev -> castorclassifier;
ethout :: OutputEth($broadcastJitter) -> toextdev;
fromhostdev -> fromhost :: FromHost(fake);
tohost :: ToHost() -> tohostdev;
