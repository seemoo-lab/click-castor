/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

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

castorclassifier :: CastorClassifier(fake, neighbors)
ratelimiter :: CastorRateLimiter($ratelimitEnable, ratelimits, $bucketSize);

replaystore :: ReplayStore($replayInterval, $replayJitter, $replayCount);

fromextdev -> castorclassifier;
ethout :: OutputEth($broadcastJitter) -> toextdev;
fromhostdev -> fromhost :: FromHost(fake);
tohost :: ToHost() -> tohostdev;

// How to choose next hop
routeselector :: CastorRouteSelectorOriginal(routingtable, neighbors, continuousflow, timeouttable, $broadcastAdjust, $tiebreakerRtt, $unicastThreshold, $rawReliabilityForBroadcast);

// How to handle PKTs and ACKs
handlepkt :: CastorHandlePkt(fake, routeselector, routingtable, flowtable, timeouttable, ratelimits, history, crypto);
handleack :: CastorHandleAck(fake, routingtable, flowtable, continuousflow, timeouttable, ratelimits, history, neighbors, crypto);

handleIpPacket :: CastorHandleIpPacket(fake, flowmanager, flowtable, crypto);
removeHeader :: CastorRemoveHeader;
