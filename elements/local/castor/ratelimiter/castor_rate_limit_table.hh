#ifndef CLICK_CASTOR_RATE_LIMIT_TABLE_HH
#define CLICK_CASTOR_RATE_LIMIT_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "../node_id.hh"
#include "castor_rate_limit.hh"

CLICK_DECLS

class CastorRateLimiter;

class CastorRateLimitTable : public Element {
public:
	CastorRateLimitTable() : _listener(NULL) {}

	const char *class_name() const { return "CastorRateLimitTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	CastorRateLimit& lookup(const NodeId& node);
	void register_listener(CastorRateLimiter*);
	void notify(const NodeId&) const;
private:
	HashTable<const NodeId, CastorRateLimit> _table;

	CastorRateLimiter* _listener;
};

CLICK_ENDDECLS

#endif
