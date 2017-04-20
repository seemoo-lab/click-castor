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
#include <click/args.hh>
#include "add_neighbor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int AddNeighbor::configure(Vector<String>& conf, ErrorHandler* errh) {
	return Args(conf, this, errh)
			.read_mp("NEIGHBORS", ElementCastArg("Neighbors"), neighbors)
			.read_or_set_p("ENABLE", enabled, true)
			.complete();
}

Packet* AddNeighbor::simple_action(Packet* p) {
	if (enabled)
		neighbors->add(CastorAnno::src_id_anno(p));

	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AddNeighbor)
