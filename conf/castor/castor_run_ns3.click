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
