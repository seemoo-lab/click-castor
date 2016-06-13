#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "castor_rate_limit_table.hh"
#include "castor_rate_limiter.hh"

CLICK_DECLS

CastorRateLimitTable::CastorRateLimitTable() : _table(NULL), _listener(NULL), warn_count(0) {}
CastorRateLimitTable::~CastorRateLimitTable() {
	delete _table;
}

int CastorRateLimitTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (Args(conf, this, errh)
		.read_or_set("MIN",  min_rate,    1)
		.read_or_set("MAX",  max_rate,  100)
		.read_or_set("INIT", init_rate,   1)
		.read_or_set("INC",  sigma_increase, 2.0)
		.read_or_set("DEC",  sigma_decrease, 0.5)
		.complete() < 0)
		return -1;
	if (init_rate < min_rate ||	init_rate > max_rate)
		return errh->error("INIT: out of bounds: %u", init_rate);
	return 0;
}

int CastorRateLimitTable::initialize(ErrorHandler*) {
	_table = new HashTable<const NeighborId, CastorRateLimit>(CastorRateLimit(init_rate, min_rate, max_rate, sigma_decrease, sigma_increase));
	return 0;
}

CastorRateLimit& CastorRateLimitTable::lookup(const NeighborId& node) {
	return (*_table)[node];
}

void CastorRateLimitTable::register_listener(CastorRateLimiter* element) {
	_listener = element;
}

void CastorRateLimitTable::notify(const NeighborId& node) const {
	if (_listener)
		_listener->update(node);
	//else if (warn_count++ < 5)
	//	click_chatter("CastorRateLimitTable::notify() called but no listener set.");
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRateLimitTable)
