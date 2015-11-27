#ifndef CLICK_CASTOR_RATE_LIMIT_TABLE_HH
#define CLICK_CASTOR_RATE_LIMIT_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "../../neighbordiscovery/neighbor_id.hh"
#include "castor_rate_limit.hh"

CLICK_DECLS

class CastorRateLimiter;

class CastorRateLimitTable : public Element {
public:
	const char *class_name() const { return "CastorRateLimitTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	CastorRateLimit& lookup(const NeighborId& node);
	void register_listener(CastorRateLimiter*);
	void notify(const NeighborId&) const;
private:
	HashTable<const NeighborId, CastorRateLimit> _table;

	CastorRateLimiter* _listener;
};

CLICK_ENDDECLS

#endif
