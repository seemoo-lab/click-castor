#ifndef CLICK_CASTOR_HISTORY_HH
#define CLICK_CASTOR_HISTORY_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "castor.hh"

CLICK_DECLS

class CastorHistory: public Element {
public:
	CastorHistory();
	~CastorHistory();

	const char *class_name() const { return "CastorHistory"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }

	void addPkt(const PacketId&, const FlowId&, IPAddress nextHop, IPAddress destination);
	bool addAckFor(const PacketId&, IPAddress prevHop);

	bool hasPkt(const PacketId&) const;
	bool hasAck(const PacketId&) const;
	bool hasAckFrom(const PacketId&, IPAddress) const;
	size_t getAcks(const PacketId&) const;

	const FlowId& getFlowId(const PacketId&) const;
	IPAddress getDestination(const PacketId&) const;
	IPAddress routedTo(const PacketId&) const;

	bool isExpired(const PacketId&) const;
	void setExpired(const PacketId&);

private:
	typedef long Key; // XXX currently using only part of pid as key
	typedef struct CastorHistoryEntry {
		FlowId fid;
		IPAddress destination; // Indicates Xcast subflow
		IPAddress nextHop;
		Vector<IPAddress> recievedACKs;
		bool expired;
	} CastorHistoryEntry;

	inline Key pidToKey(const PacketId& pid) const;
	inline const CastorHistoryEntry* getEntry(const PacketId&) const;
	inline CastorHistoryEntry* getEntry(const PacketId&);

	HashTable<Key, CastorHistoryEntry> history;
};

CLICK_ENDDECLS

#endif
