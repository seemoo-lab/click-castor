/**************
 * Parameters *
 **************/

define(
	$CASTORTYPE 201,

	/** Castor parameters (settings from experimental setup of Castor technical report) **/
	$broadcastAdjust 8.0, // bandwidth investment for route discovery (larger values reduce the broadcast probability)
	$updateDelta 0.8, // adaptivity of the reliability estimators

	/** Timeout boundaries */
	$initTo 1000, // 1 second
	$minTo   100, // 100 ms
	$maxTo 60000, // 1 minute

	/** Neighbor discovery */
	$neighborsEnable true, // whether or not to use neighbor beacons; if set to false, settings for neighborTimeout and beaconingInterval are irrelevant
	$neighborTimeout 4500, // in milliseconds (4 * beaconingInterval + delta)
	$beaconingInterval 1000, // in milliseconds

	$broadcastJitter 1000, // jitter in microseconds to avoid collisions for broadcast traffic, only relevant when running under ns-3
	
	$headroom 544, // Fixed Xcastor header + 10 * 29 bytes per destination + IP header + Ethernet header
);

require(
	library castor_io_ns3.click,
	//library castor_io_userlevel.click,
);
