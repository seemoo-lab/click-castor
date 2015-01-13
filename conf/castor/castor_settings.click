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
	$timeout 500, // in milliseconds
	$neighborTimeout 1000, // in milliseconds
	$beaconingInterval 250, // in milliseconds

	$broadcastJitter 100, // jitter in microseconds to avoid collisions for broadcast traffic
	$unicastJitter 0, // to add some artificial processing delay
	
	$numNodes 200,
);

AddressInfo(fake $EthDev);
AddressInfo(netAddr 10.0.0.0)

// Need an IP routing table for ns-3 (dummy)
rt :: StaticIPLookup(0.0.0.0/0 0);
Idle() -> rt -> Discard;