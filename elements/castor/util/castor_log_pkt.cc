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
#include "castor_log_pkt.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorLogPkt::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_or_set_p("ACTIVE", active, true)
			.read_or_set("READ_ENTRIES_MAX", read_entries_max, 100)
			.complete();
}

int CastorLogPkt::initialize(ErrorHandler*) {
	reset();
	return 0;
}

Packet* CastorLogPkt::simple_action(Packet *p) {
	if (active)
	if (CastorPacket::getType(p) == CastorType::PKT) {
		CastorPkt& pkt = (CastorPkt&) *p->data();

		EntryListNode *e = new EntryListNode();
		e->timestamp = Timestamp::now();
		e->src = pkt.src;
		e->dst = pkt.dst;
		e->fid = pkt.fid;
		e->pid = pkt.pid;
		e->kpkt = ntohs(pkt.kpkt);
		e->size = p->length();
		e->this_node = CastorAnno::src_id_anno(p);
		e->next_hop = CastorAnno::dst_id_anno(p);

		entries.push_back(e);
	}
	return p;
}

void CastorLogPkt::reset() {
	entries.clear();
}

void CastorLogPkt::read_entry(const EntryListNode& e, StringAccum& sa) {
	sa << e.timestamp << " ";
	sa << e.src.unparse() << " ";
	sa << e.dst.unparse() << " ";
	sa << e.fid.str() << " ";
	sa << e.pid.str() << " ";
	sa << e.kpkt << " ";
	sa << e.size << " ";
	sa << e.this_node << " ";
	sa << e.next_hop;
}

String CastorLogPkt::read_handler(Element *e, void *thunk) {
	CastorLogPkt* recorder = static_cast<CastorLogPkt*>(e);

	switch(reinterpret_cast<uintptr_t>(thunk)) {
	case Statistics::total:
		return String(recorder->entries.size());
	case Statistics::entry:
		if (recorder->entries.empty()) {
			return String();
		} else {
			EntryListNode* e = recorder->entries.front();
			recorder->entries.pop_front();
			StringAccum sa;
			read_entry(*e, sa);
			delete e;
			return sa.take_string();
		}
	case Statistics::entries:
	{
		StringAccum sa;
		for (int i = 0; i < recorder->read_entries_max && !recorder->entries.empty(); i++) {
			EntryListNode* e = recorder->entries.front();
			recorder->entries.pop_front();
			read_entry(*e, sa);
			sa << "\n";
			delete e;
		}
		return sa.take_string();
	}
	default:
		click_chatter("enum error");
		return String();
	}
}

void CastorLogPkt::add_handlers() {
	add_read_handler("total", read_handler, Statistics::total);
	add_read_handler("entry", read_handler, Statistics::entry);
	add_read_handler("entries", read_handler, Statistics::entries);
	add_data_handlers("max_read", Handler::f_read | Handler::f_write, &this->read_entries_max);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorLogPkt)
