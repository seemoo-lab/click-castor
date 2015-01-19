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
	CastorHistory();

	const char *class_name() const { return "CastorHistory"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }

	void addPkt(const PacketId&, const FlowId&, IPAddress prevHop, IPAddress nextHop, IPAddress destination);
	bool addFirstAckForCastor(const PacketId&, IPAddress prevHop, const ACKAuth&);
	bool addFirstAckForXcastor(const PacketId&, IPAddress prevHop, const EACKAuth&);
	bool addAckFor(const PacketId&, IPAddress prevHop);

	bool hasPkt(const PacketId&) const;
	bool hasPktFrom (const PacketId&, IPAddress) const;
	bool hasAck(const PacketId&) const;
	bool hasAckFrom(const PacketId&, IPAddress) const;
	size_t getPkts(const PacketId&) const;
	const Vector<IPAddress>& getPktSenders(const PacketId&) const;
	size_t getAcks(const PacketId&) const;

	const FlowId& getFlowId(const PacketId&) const;
	IPAddress getDestination(const PacketId&) const;
	const EACKAuth& getEAckAuth(const PacketId&) const;
	const ACKAuth& getAckAuth(const PacketId&) const;
	IPAddress routedTo(const PacketId&) const;

	bool isExpired(const PacketId&) const;
	void setExpired(const PacketId&);

private:
	typedef struct CastorHistoryEntry {
		// PIDs
		FlowId fid;
		IPAddress destination; // Indicates Xcast subflow
		Vector<IPAddress> prevHops;
		IPAddress nextHop;

		// ACKs
		Vector<IPAddress> recievedACKs;
		Hash auth;
		bool expired;
	} CastorHistoryEntry;

	inline const CastorHistoryEntry* getEntry(const PacketId&) const;
	inline CastorHistoryEntry* getEntry(const PacketId&);

	HashTable<Hash, CastorHistoryEntry> history;
};

CLICK_ENDDECLS

#endif
