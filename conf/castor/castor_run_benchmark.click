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

// I/O interfaces for local benchmark
AddressInfo(fake 1.2.3.4 C0-FF-EE-C0-FF-EE);

fromhostdev :: { RatedSource(LENGTH 1024, RATE 100000000, LIMIT -1) -> IPEncap(4, 1.2.3.4, 4.3.2.1) -> output };
tohostdev :: Discard;
fromextdev :: Idle;
toextdev :: Unqueue
	-> Classifier(12/88B6, -)
	=> ( input[0] -> Strip(22) -> CastorRecordPkt(VERBOSE true) -> output; 
	     input[1] -> output; )
	-> Discard;

elementclass ProcessingJitter {
	$jitterMin, $jitterMax |

	input -> output;
}

require(
	library castor_init_blocks.click,
	library castor_wiring.click,
);
