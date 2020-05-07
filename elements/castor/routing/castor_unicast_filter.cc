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
#include <click/hashtable.hh>
#include "castor_unicast_filter.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorUnicastFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_or_set_p("ACTIVE", active, false)
			.complete();
}

Packet* CastorUnicastFilter::simple_action(Packet *p) {
	if (!active)
		return p;

	if (CastorAnno::dst_id_anno(p) == NeighborId::make_broadcast()) {
		return p;
	} else {
		checked_output_push(1, p);
		return 0;
	}
}

void CastorUnicastFilter::add_handlers() {
	add_data_handlers("active", Handler::f_read | Handler::f_write, &this->active);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUnicastFilter)
