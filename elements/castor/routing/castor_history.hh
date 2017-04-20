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

#ifndef CLICK_CASTOR_HISTORY_HH
#define CLICK_CASTOR_HISTORY_HH

#include <click/element.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
#include <click/timestamp.hh>
#include "../castor.hh"
#include "../neighbor_id.hh"

CLICK_DECLS

class CastorHistory: public Element {
public:
	const char *class_name() const { return "CastorHistory"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }

	void addPkt(const PacketId&, unsigned int, const FlowId&, const NeighborId& prevHop, const NeighborId& nextHop, const NodeId& source, const NodeId& destination, Timestamp timestamp);
	bool addFirstAckFor(const PacketId&, const NeighborId& prevHop, const AckAuth&);
	bool addAckFor(const PacketId&, const NeighborId& prevHop);

	bool hasPkt(const PacketId&) const;
	bool hasPktFrom (const PacketId&, const NeighborId&) const;
	bool hasAck(const PacketId&) const;
	bool hasAckFrom(const PacketId&, const NeighborId&) const;
	const Vector<NeighborId>& getPktSenders(const PacketId&) const;

	unsigned int k(const PacketId&) const;
	const FlowId& getFlowId(const PacketId&) const;
	const NodeId& getSource(const PacketId&) const;
	const NodeId& getDestination(const PacketId&) const;
	const AckAuth& getAckAuth(const PacketId&) const;
	const NeighborId& routedTo(const PacketId&) const;

	bool remove(const PacketId&);

	const Timestamp& getTimestamp(const PacketId&) const;
private:
	class CastorHistoryEntry {
	public:
		Timestamp timestamp;

		// PIDs
		unsigned int k;
		FlowId fid;
		NodeId source;
		NodeId destination; // Indicates Xcast subflow
		Vector<NeighborId> prevHops;
		NeighborId nextHop;

		// ACKs
		Vector<NeighborId> recievedACKs;
		AckAuth auth;
	};

	inline const CastorHistoryEntry* getEntry(const PacketId&) const;
	inline CastorHistoryEntry* getEntry(const PacketId&);

	HashTable<Hash, CastorHistoryEntry> history;
};

CLICK_ENDDECLS

#endif
