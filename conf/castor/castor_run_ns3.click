// I/O interfaces for ns-3
define(
	$EthDev eth0,
	$HostDev tap0,
);

AddressInfo(fake $EthDev);

fromhostdev :: FromSimDevice($HostDev, SNAPLEN 4096, HEADROOM $headroom);
tohostdev :: ToSimDevice($HostDev, IP);
fromextdev :: FromSimDevice($EthDev, SNAPLEN 4096, PROMISC true)
toextdev :: ToSimDevice($EthDev);

// Need an IP routing table for ns-3 (dummy)
rt :: StaticIPLookup(0.0.0.0/0 0);
Idle() -> rt -> Discard;

/**
 * Delays MAC layer broadcast frames by a random jitter. Due to perfect synchronization among the nodes
 * we otherwise get collisions.
 */
elementclass BroadcastJitter {
	$broadcastJitter |

	input
		-> cp :: CheckPaint(10, ANNO 38) // this frame was broadcasted -> jitter transmission (Paint(10))
		-> Queue
		-> JitterUnqueue($broadcastJitter, true) // 'true' set for simulator -> much better performance
		-> output;

	cp[1]
		-> output; // this frame was not broadcasted -> no need to delay (Paint(0))
}

require(
	library castor_socket.click,
	library castor_init_blocks.click,
	library castor_wiring.click,
);
