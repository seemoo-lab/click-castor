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

/**************
 * Parameters *
 **************/

define(
	// 0x88B5 and 0x88B6 reserved for private experiments, so we use them
	$ETHERTYPE_CASTOR 0x88B6,
	$ETHERTYPE_BEACON 0x88B5,

	/** Castor parameters (settings from experimental setup of Castor technical report) **/
	$broadcastAdjust 8.0, // bandwidth investment for route discovery (larger values reduce the broadcast probability)
	$updateDelta 0.8, // adaptivity of the reliability estimators

	$rtTimeout 256000, // rt entry timeout (ms)
	$rtCleanInterval 1000, // how often is rt cleaned (ms)

	$flowSize 256, // number of pids per flow

	/** Timeout boundaries (in s) */
	$initTo 0.3,
	$minTo  0.1, // if the value is too low, might result in routing loops due to unintentional replay attack
	$maxTo  0.3,

	/** Rate limit boundaries (in PKTs/sec) */
	$ratelimitEnable false,
	$initRate   4,
	$minRate    1,
	$maxRate 1024,
	$bucketSize 4, // how many packets we queue per neighbor before starting to drop new ones

	/** Neighbor discovery */
	$neighborsEnable true, // whether or not to use neighbor beacons; if set to false, settings for neighborTimeout and beaconingInterval are irrelevant
	$neighborTimeout  1000, // in milliseconds (4 * beaconingInterval)
	$beaconingInterval 250, // in milliseconds

	/** v2 optimizations */
	$fullFlowAuth false,
	$forceNonce   false,
	$replayProtect true,
	$copyEstimators false,
	$useFirstAck false, /* give preference to first ACK */
	$tiebreakerRtt true, /* tie breaks based on RTT instead of randomly */
	$unicastThreshold 0.99, /* start to always unicast from this threshold */
	$rawReliabilityForBroadcast true, /* use 'raw' neighbor reliability for broadcast decision */

	/* jitter in microseconds to avoid collisions due to perfect synchronization and model processing delay */
	$jitterMin 100,
	$jitterMax 150,

	$headroom 224, // Castor header + IP header + Ethernet header

	/** Attacks */
	$replayInterval  200, // in ms
	$replayJitter      0, // in ms
	$replayCount      10, // how often a PKT-ACK pair will be replayed
	$replayRateMax    10, // how many PKT-ACK pairs will be emitted per s
);
