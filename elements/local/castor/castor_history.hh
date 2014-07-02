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

	void addPKT(const PacketId&, const FlowId&, const IPAddress& nextHop);
	bool addACKFor(const PacketId&, const IPAddress& prevHop);

	bool hasPkt(const PacketId&) const;
	bool hasACK(const PacketId&) const;
	bool hasACK(const PacketId&, const IPAddress&) const;
	size_t getACKs(const PacketId&) const;

	const FlowId& getFlowId(const PacketId&) const;
	const IPAddress& routedTo(const PacketId&) const;

	bool isExpired(const PacketId&) const;
	void setExpired(const PacketId&);

private:
	typedef long Key; // FIXME currently using only part of pid as key
	typedef struct {
		FlowId fid;
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
