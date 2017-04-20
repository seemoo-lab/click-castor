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
#include "forwarder_filter.hh"
#include "forwarder_list.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int ForwarderFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("ID", my_id)
			.read_or_set_p("IGNORE", ignore, false)
			.complete();
}

Packet* ForwarderFilter::simple_action(Packet *p) {
	const ForwarderList& fl = reinterpret_cast<const ForwarderList&>(*p->data());
	if (ignore || fl.node == my_id || fl.node == NeighborId::make_broadcast()) {
		CastorAnno::dst_id_anno(p) = fl.node;
		return p;
	} else {
		checked_output_push(1, p);
		return 0;
	}
}

int ForwarderFilter::write_handler(const String &str, Element *e, void *, ErrorHandler *errh) {
	ForwarderFilter* filter = (ForwarderFilter*) e;

	return Args(filter, errh).push_back_words(str)
			.read_mp("IGNORE", filter->ignore)
			.complete() < 0;
}

void ForwarderFilter::add_handlers() {
	add_write_handler("IGNORE", write_handler, 0);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ForwarderFilter)
