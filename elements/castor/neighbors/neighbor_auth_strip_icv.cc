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

#include <click/config.h>
#include "neighbor_auth_strip_icv.hh"
#include "forwarder_list.hh"
#include "../castor.hh"

CLICK_DECLS

Packet* NeighborAuthStripICV::simple_action(Packet* p) {
	const ForwarderList& fl = *reinterpret_cast<const ForwarderList*>(p->data());
	p->take(fl.nicv * icv_BYTES);
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NeighborAuthStripICV)
