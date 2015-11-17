#ifndef CLICK_CASTOR_RATE_LIMIT_TABLE_HH
#define CLICK_CASTOR_RATE_LIMIT_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "../node_id.hh"
#include "castor_rate_limit.hh"

CLICK_DECLS

class CastorRateLimitTable : public Element {
public:
	const char *class_name() const { return "CastorRateLimitTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	CastorRateLimit& lookup(const NodeId& node);
private:
	HashTable<const NodeId, CastorRateLimit> _table;
};

CLICK_ENDDECLS

#endif
