#include <click/config.h>
#include "castor_rate_limit.hh"

CLICK_DECLS

void CastorRateLimit::increase() {
	rate = bound(rate * sigma_increase);
}

void CastorRateLimit::decrease() {
	rate = bound(rate * sigma_increase);
}

CastorRateLimit::rate_t CastorRateLimit::value() const {
	return rate;
}

CastorRateLimit::rate_t CastorRateLimit::bound(rate_t proposed_rate) const {
	return (proposed_rate < min_rate) ? min_rate :
		   (proposed_rate > max_rate) ? max_rate :
	       /* else */				    proposed_rate;
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(CastorRateLimit)
