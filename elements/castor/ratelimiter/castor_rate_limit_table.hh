#ifndef CLICK_CASTOR_RATE_LIMIT_TABLE_HH
#define CLICK_CASTOR_RATE_LIMIT_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "../neighbor_id.hh"
#include "castor_rate_limit.hh"

CLICK_DECLS

class CastorRateLimiter;

class CastorRateLimitTable : public Element {
public:
	CastorRateLimitTable();
	virtual ~CastorRateLimitTable();

	const char *class_name() const { return "CastorRateLimitTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);
	int initialize(ErrorHandler*);

	CastorRateLimit& lookup(const NeighborId& node);
	void register_listener(CastorRateLimiter*);
	void notify(const NeighborId&) const;
private:
	unsigned int init_rate;
	unsigned int min_rate;
	unsigned int max_rate;
	double sigma_decrease;
	double sigma_increase;

	HashTable<const NeighborId, CastorRateLimit>* _table;
	CastorRateLimiter* _listener;

	mutable unsigned int warn_count;
};

CLICK_ENDDECLS

#endif
