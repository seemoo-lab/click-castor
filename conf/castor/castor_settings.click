/**************
 * Parameters *
 **************/

define(
	/** Device names for ns3 **/
	$EthDev eth0,
	$HostDev tap0,

	$CASTORTYPE 201,

	/** Castor parameters (settings from experimental setup of Castor technical report) **/
	$broadcastAdjust 8.0, // bandwidth investment for route discovery (larger values reduce the broadcast probability)
	$updateDelta 0.8, // adaptivity of the reliability estimators
	$timeout 200, // in milliseconds

	$neighborsEnable true, // whether or not to use neighbor beacons; if set to false, settings for neighborTimeout and beaconingInterval are irrelevant
	$neighborTimeout 4500, // in milliseconds (4 * beaconingInterval + delta)
	$beaconingInterval 1000, // in milliseconds

	$broadcastJitter 100, // jitter in microseconds to avoid collisions for broadcast traffic
	$unicastJitter 0, // to add some artificial processing delay
	
	$headroom 544, // Fixed Xcastor header + 10 * 29 bytes per destination + IP header + Ethernet header
);

AddressInfo(fake $EthDev);

// Need an IP routing table for ns-3 (dummy)
rt :: StaticIPLookup(0.0.0.0/0 0);
Idle() -> rt -> Discard;