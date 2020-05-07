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

#ifndef CLICK_CASTOR_LOG_PKT_HH
#define CLICK_CASTOR_LOG_PKT_HH

#include <click/element.hh>
#include <click/timestamp.hh>
#include <click/list.hh>
#include <click/straccum.hh>
#include "../castor.hh"

CLICK_DECLS

class CastorLogPkt: public Element {
public:
	const char *class_name() const { return "CastorLogPkt"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);
	int initialize(ErrorHandler*);

	virtual Packet* simple_action(Packet *);
	void reset();

	void add_handlers();

private:
	struct EntryListNode {
		inline EntryListNode() {}
		List_member<EntryListNode> node;
		Timestamp timestamp;
		NodeId src;
		NodeId dst;
		FlowId fid;
		PacketId pid;
		uint16_t kpkt;
		uint32_t size;
		EtherAddress this_node;
		EtherAddress next_hop;
	};

	List<EntryListNode, &EntryListNode::node> entries;

	bool active;

	int read_entries_max;

	static void read_entry(const EntryListNode& e, StringAccum& sa);
	static String read_handler(Element*, void*);

	struct Statistics {
		enum {
			total,
			entry,
			entries,
		};
	};

};

CLICK_ENDDECLS

#endif
