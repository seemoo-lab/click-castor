#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "castor_rate_limit_table.hh"
#include "castor_rate_limiter.hh"

CLICK_DECLS

CastorRateLimit::rate_t CastorRateLimit::min_rate;
CastorRateLimit::rate_t CastorRateLimit::max_rate;
CastorRateLimit::rate_t CastorRateLimit::init_rate;
double CastorRateLimit::sigma_increase;
double CastorRateLimit::sigma_decrease;

int CastorRateLimitTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (Args(conf, this, errh)
		.read_or_set("MIN",  CastorRateLimit::min_rate,    1)
		.read_or_set("MAX",  CastorRateLimit::max_rate,  100)
		.read_or_set("INIT", CastorRateLimit::init_rate,   1)
		.read_or_set("INC",  CastorRateLimit::sigma_increase, 2.0)
		.read_or_set("DEC",  CastorRateLimit::sigma_decrease, 0.5)
		.complete() < 0)
		return -1;
	if (CastorRateLimit::init_rate < CastorRateLimit::min_rate ||
		CastorRateLimit::init_rate > CastorRateLimit::max_rate)
		return errh->error("INIT: out of bounds: %u", CastorRateLimit::init_rate);
	_listener = NULL;
	warn_count = 0;
	return 0;
}

CastorRateLimit& CastorRateLimitTable::lookup(const NeighborId& node) {
	return _table[node];
}

void CastorRateLimitTable::register_listener(CastorRateLimiter* element) {
	_listener = element;
}

void CastorRateLimitTable::notify(const NeighborId& node) const {
	if (_listener)
		_listener->update(node);
	else if (warn_count++ < 5)
		click_chatter("CastorRateLimitTable::notify() called but no listener set.");
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRateLimitTable)
