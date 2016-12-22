/****************************
 * Initialize common blocks *
 ****************************/
require(
	library castor_settings.click,
	library castor_io.click,
	library castor_common.click,
);

sam::SAManagement(fake, fake);
crypto::Crypto(sam);
flowtable :: CastorFlowTable(TIMEOUT $rtTimeout, CLEAN $rtCleanInterval);
flowmanager :: CastorFlowManager($flowSize, flowtable, crypto);

neighbors :: Neighbors($neighborTimeout, $neighborsEnable);
history :: CastorHistory;
routingtable :: CastorRoutingTable($updateDelta, TIMEOUT $rtTimeout, CLEAN $rtCleanInterval);
timeouttable :: CastorTimeoutTable(INIT $initTo, MIN $minTo, MAX $maxTo);
ratelimits :: CastorRateLimitTable(INIT $initRate, MIN $minRate, MAX $maxRate);
continuousflow :: CastorContinuousFlowMap();
debug_handler :: CastorDebugHandler(flowmanager);

castorclassifier :: CastorClassifier(fake, neighbors)
ratelimiter :: CastorRateLimiter($ratelimitEnable, ratelimits, $bucketSize);

replaystore :: ReplayStore($replayInterval, $replayJitter, $replayCount);

fromextdev -> castorclassifier;
ethout :: OutputEth($broadcastJitter) -> toextdev;
fromhostdev -> fromhost :: FromHost(fake);
tohost :: ToHost() -> tohostdev;

// How to choose next hop
routeselector :: CastorRouteSelectorOriginal(routingtable, neighbors, continuousflow, $broadcastAdjust);

// How to handle PKTs and ACKs
handlepkt :: CastorHandlePkt(fake, routeselector, routingtable, flowtable, timeouttable, ratelimits, history, crypto);
handleack :: CastorHandleAck(fake, routingtable, flowtable, continuousflow, timeouttable, ratelimits, history, neighbors, crypto);

handleIpPacket :: CastorHandleIpPacket(fake, flowmanager, flowtable, crypto);
removeHeader :: CastorRemoveHeader;
