
/** CLICK script for use with real linux systems **/

require(library aodv_core.click);

define(
	$EthDev wlan0,
	$HostDev tun0,
);

AddressInfo(fake $EthDev);

elementclass ToNetwork{
	input[0]
		-> Queue(2000)
		-> ToDevice(wlan0);
}

elementclass FromNetwork{
	$myaddr_ethernet |
	FromDevice(wlan0)
		-> HostEtherFilter($myaddr_ethernet, DROP_OWN false, DROP_OTHER true)
		-> output;
}

FromNetwork(fake) -> aodv_core::AODV_Core(fake) -> output::ToNetwork;

host :: KernelTun(fake:ip/16, DEVNAME $HostDev)
	-> [1]aodv_core;

aodv_core[1] 
	-> host;
