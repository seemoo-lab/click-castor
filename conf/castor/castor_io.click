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

/**
 * Handle output to the Ethernet device
 */
elementclass OutputEth { 
	$broadcastJitter |

	prio :: PrioSched
		-> output

	highprio :: Queue -> [0] prio;
	lowprio :: Queue -> [1] prio;
	beacons_be :: Queue(CAPACITY 1) // best effort, size of one since we don't need beacon 'floods'
		-> recBeacon :: CastorRecordPkt
		-> [2] prio;

	input[0] // Castor PKT
		-> ProcessingJitter($jitterMin, $jitterMax)
		-> lowprio;

	input[1] // Castor ACK
		-> ProcessingJitter($jitterMin, $jitterMax)
		-> highprio;

	input[2] // Beacons
		-> ProcessingJitter($jitterMin, $jitterMax)
		-> beacons_be;
}

/**
 * Handle packets destined for local host
 *
 * Input(0): IP packet
 * Input(1): Other packet
 */
elementclass ToHost {

	input[0]
		-> CheckIPHeader
		-> output;

}

/**
 * Handle incoming packets from local host
 */
elementclass FromHost {
	$myIP |

	input
		-> CheckIPHeader
		-> SetIPSrc($myIP) // Packets coming from ns-3 tun0 (host) device have 127.0.0.1 set as source address
		-> output;

}
