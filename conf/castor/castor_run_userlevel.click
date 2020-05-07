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

// I/O interfaces for userlevel drivers
define(
	$EthDev wlan0,
	$HostDev tun0,
	$WormholeDev eth0,
);

AddressInfo(fake $EthDev);

tun :: KernelTun(fake:ip/16, HEADROOM $headroom, DEVNAME $HostDev, MTU 500);
tun -> fromhostdev :: { input -> output };
tohostdev :: { input -> output; } -> tun;
fromextdev :: { FromDevice($EthDev, SNAPLEN 4096, PROMISC true, SNIFFER false) -> output;
                FromDevice($WormholeDev, SNAPLEN 4096, PROMISC true, SNIFFER true) -> wormhole :: Suppressor(active0 false) -> output; }
toextdev :: { input -> ToDevice($EthDev) -> wormhole :: Suppressor(active0 false) -> Queue() -> ToDevice($WormholeDev); }

/**
 * Dummy class, does nothing as we do not need artificial jitter on 'real' devices
 */
elementclass ProcessingJitter {
	$jitterMin, $jitterMax |

	input -> output;
}

// Finally wire all blocks
require(
	library castor_socket.click,
	library castor_init_blocks.click,
	library castor_wiring.click,
);
