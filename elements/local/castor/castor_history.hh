#ifndef CLICK_CASTOR_HISTORY_HH
#define CLICK_CASTOR_HISTORY_HH

#include <click/element.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
#include "castor.hh"
#include "castor_xcast.hh"

CLICK_DECLS

class CastorHistory: public Element {
public:
	const char *class_name() const { return "CastorHistory"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }

	void addPkt(const PacketId&, const FlowId&, NodeId prevHop, NodeId nextHop, NodeId destination);
	bool addFirstAckForCastor(const PacketId&, NodeId prevHop, const AckAuth&);
	bool addFirstAckForXcastor(const PacketId&, NodeId prevHop, const PktAuth&);
	bool addAckFor(const PacketId&, NodeId prevHop);

	bool hasPkt(const PacketId&) const;
	bool hasPktFrom (const PacketId&, NodeId) const;
	bool hasAck(const PacketId&) const;
	bool hasAckFrom(const PacketId&, NodeId) const;
	size_t getPkts(const PacketId&) const;
	const Vector<NodeId>& getPktSenders(const PacketId&) const;
	size_t getAcks(const PacketId&) const;

	const FlowId& getFlowId(const PacketId&) const;
	NodeId getDestination(const PacketId&) const;
	const PktAuth& getEAckAuth(const PacketId&) const;
	const AckAuth& getAckAuth(const PacketId&) const;
	NodeId routedTo(const PacketId&) const;

	bool isExpired(const PacketId&) const;
	void setExpired(const PacketId&);

private:
	typedef struct CastorHistoryEntry {
		// PIDs
		FlowId fid;
		NodeId destination; // Indicates Xcast subflow
		Vector<NodeId> prevHops;
		NodeId nextHop;

		// ACKs
		Vector<NodeId> recievedACKs;
		Hash auth;
		bool expired;
	} CastorHistoryEntry;

	inline const CastorHistoryEntry* getEntry(const PacketId&) const;
	inline CastorHistoryEntry* getEntry(const PacketId&);

	HashTable<Hash, CastorHistoryEntry> history;
};

CLICK_ENDDECLS

#endif
