#ifndef CLICK_CASTOR_NEXT_FLOW_TABLE_HH
#define CLICK_CASTOR_NEXT_FLOW_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "castor.hh"

CLICK_DECLS

class CastorNextFlowTable : public Element {
public:
	const char *class_name() const { return "CastorNextFlowTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }

	/**
	 * Sets a new mapping. Returns true if the map was changed.
	 * Returns false if the mapping already exists.
	 */
	bool set(const NextFlowAuth& nfauth, const FlowId& fid, NodeId subflow);
	void remove(const NextFlowAuth& nfauth, NodeId subflow);
	const FlowId* get(const NextFlowAuth& nfauthm, NodeId subflow) const;
	void print(const NextFlowAuth& nfauth);
private:
	class Entry {
	public:
		FlowId fid;
		HashTable<NodeId, NodeId> subflows;
	};
	HashTable<NextFlowAuth, Entry> map;
};

CLICK_ENDDECLS

#endif
