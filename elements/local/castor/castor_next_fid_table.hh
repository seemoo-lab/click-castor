#ifndef CLICK_CASTOR_NEXT_FID_TABLE_HH
#define CLICK_CASTOR_NEXT_FID_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "castor.hh"

CLICK_DECLS

class CastorNextFidTable : public Element {
public:
	const char *class_name() const { return "CastorNextFidTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }

	/**
	 * Sets a new mapping. Returns true if the map was changed.
	 * Returns false if the mapping already exists.
	 */
	bool set(const NextFlowAuth& nfauth, const FlowId& fid);
	const FlowId* get(const NextFlowAuth& nfauth);
private:
	HashTable<NextFlowAuth, FlowId> map;
};

CLICK_ENDDECLS

#endif
