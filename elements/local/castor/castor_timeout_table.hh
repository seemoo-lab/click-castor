#ifndef CLICK_CASTOR_TIMEOUT_TABLE_HH
#define CLICK_CASTOR_TIMEOUT_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "castor.hh"
#include "castor_timeout.hh"

CLICK_DECLS

class CastorTimeoutTable : public Element {
public:
	typedef NodeId SubflowId;

	const char *class_name() const { return "CastorTimeoutTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	CastorTimeout& getTimeout(const FlowId& flow, const SubflowId& subflow, NodeId forwarder);

private:
	typedef HashTable<NodeId, CastorTimeout> ForwarderEntry;
	typedef HashTable<SubflowId, ForwarderEntry> SubflowEntry;
	typedef HashTable<FlowId, SubflowEntry> FlowEntry;
	FlowEntry flows;

	template <typename K, typename V>
	static V& getEntryInsertDefault(HashTable<K, V>& map, const K& key);
};

CLICK_ENDDECLS

#endif
