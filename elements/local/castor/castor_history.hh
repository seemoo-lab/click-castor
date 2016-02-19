#ifndef CLICK_CASTOR_HISTORY_HH
#define CLICK_CASTOR_HISTORY_HH

#include <click/element.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
#include <click/timestamp.hh>
#include "castor.hh"
#include "../neighbordiscovery/neighbor_id.hh"

CLICK_DECLS

class CastorHistory: public Element {
public:
	const char *class_name() const { return "CastorHistory"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }

	void addPkt(const PacketId&, const FlowId&, const NeighborId& prevHop, const NeighborId& nextHop, const NodeId& destination, Timestamp timestamp);
	bool addFirstAckFor(const PacketId&, const NeighborId& prevHop, const AckAuth&);
	bool addAckFor(const PacketId&, const NeighborId& prevHop);

	bool hasPkt(const PacketId&) const;
	bool hasPktFrom (const PacketId&, const NeighborId&) const;
	bool hasAck(const PacketId&) const;
	bool hasAckFrom(const PacketId&, const NeighborId&) const;
	const Vector<NeighborId>& getPktSenders(const PacketId&) const;

	const FlowId& getFlowId(const PacketId&) const;
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
		FlowId fid;
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
