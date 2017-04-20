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

#ifndef CLICK_FORWARDER_LIST_HH
#define CLICK_FORWARDER_LIST_HH

#include "../neighbor_id.hh"

CLICK_DECLS

class ForwarderList {
public:
	ForwarderList() : num(1), nicv(0) {
		static_assert(sizeof(ForwarderList) == 8, "ForwarderList has wrong size");
	}
	uint8_t    num;  /* number of node identifiers in the list, can be extended in the future */
	uint8_t    nicv; /* number of ICVs appended to this packet,	num != nicv if 'node'
						is the broadcast address, i.e., more nodes addressed explicitly enumerated */
	NeighborId node; /* forwarding node */
} CLICK_SIZE_PACKED_ATTRIBUTE;

CLICK_ENDDECLS
#endif
