// I/O interfaces for userlevel drivers
define(
	$EthDev wlan0,
	$HostDev tun0,
);

require(
	library castor_socket.click,
);

AddressInfo(fake $EthDev);

tun :: KernelTun(fake:ip/16, HEADROOM $headroom, DEVNAME $HostDev);
tun -> fromhostdev :: { input -> output };
tohostdev :: { input -> output; } -> tun;
fromextdev :: FromDevice($EthDev, SNAPLEN 4096, PROMISC true, SNIFFER false)
toextdev :: ToDevice($EthDev);

/**
 * Dummy class, does nothing as we do not need artificial jitter on 'real' devices
 */
elementclass BroadcastJitter {
	$broadcastJitter |

	input -> output;
}
