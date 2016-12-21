#ifndef CLICK_CASTOR_FLOW_TABLE_HH
#define CLICK_CASTOR_FLOW_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "../castor.hh"
#include "castor_flow_entry.hh"

CLICK_DECLS

class CastorFlowTable : public Element {
public:
	const char *class_name() const { return "CastorFlowTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }

	CastorFlowEntry &get(const FlowId &fid);
	bool has(const FlowId &fid) const;
private:
	HashTable<FlowId, CastorFlowEntry> flows;
};

CLICK_ENDDECLS

#endif
