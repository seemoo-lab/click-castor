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

	$flowSize 256, // number of pids per flow

	/** Timeout boundaries */
	$initTo 1000, // 1 second
	$minTo   100, // 100 ms
	$maxTo 60000, // 1 minute

	/** Rate limit boundaries (in PKTs/sec) */
	$ratelimitEnable true,
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
	$copyEstimators true,

	$broadcastJitter 1000, // jitter in microseconds to avoid collisions for broadcast traffic, only relevant when running under ns-3

	$headroom 544, // Fixed Xcastor header + 10 * 29 bytes per destination + IP header + Ethernet header

	/** Attacks */
	$replayInterval 1000, // in ms
	$replayJitter    300, // in ms
	$replayCount       5, // how often a PKT-ACK pair will be replayed
);

require(
	library castor_io_ns3.click,
	//library castor_io_userlevel.click,
	//library castor_io_benchmark.click,
);
