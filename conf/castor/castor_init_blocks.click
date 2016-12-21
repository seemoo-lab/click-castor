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
flowtable :: CastorFlowTable();
flowmanager :: CastorFlowManager($flowSize, flowtable, crypto);

groupmap :: XcastDestinationMap;

neighbors :: Neighbors($neighborTimeout, $neighborsEnable);
history :: CastorHistory;
routingtable :: CastorRoutingTable($updateDelta, TIMEOUT $rtTimeout, CLEAN $rtCleanInterval);
timeouttable :: CastorTimeoutTable(INIT $initTo, MIN $minTo, MAX $maxTo);
ratelimits :: CastorRateLimitTable(INIT $initRate, MIN $minRate, MAX $maxRate);
debug_handler :: CastorDebugHandler(flowmanager);

castorclassifier :: CastorClassifier(fake, neighbors)
ratelimiter :: CastorRateLimiter($ratelimitEnable, ratelimits, $bucketSize);

replaystore :: ReplayStore($replayInterval, $replayJitter, $replayCount);

fromextdev -> castorclassifier;
ethout :: OutputEth($broadcastJitter) -> toextdev;
fromhostdev -> fromhost :: FromHost(fake);
tohost :: ToHost() -> tohostdev;

// How to choose next hop
routeselector :: CastorRouteSelectorOriginal(routingtable, neighbors, $broadcastAdjust);

// How to handle PKTs and ACKs
handlepkt :: CastorHandlePkt(fake, routeselector, routingtable, flowtable, timeouttable, ratelimits, history, crypto);
handleack :: CastorHandleAck(fake, routingtable, flowtable, timeouttable, ratelimits, history, neighbors, crypto);

handleIpPacket :: CastorHandleMulticastToUnicastIpPacket(fake, flowmanager, flowtable, crypto, groupmap);
removeHeader :: CastorRemoveHeader;
