#include <click/config.h>
#include "castor_rate_limit.hh"

CLICK_DECLS

/** Default values as proposed in the Castor paper */
double CastorRateLimit::init_rate =   1.0;
double CastorRateLimit::min_rate  =   0.1;
double CastorRateLimit::max_rate  = 100.0;
double CastorRateLimit::sigma_decrease = 0.5;
double CastorRateLimit::sigma_increase = 2.0;

void CastorRateLimit::increase() {
	rate = bound(rate * CastorRateLimit::sigma_increase);
}

void CastorRateLimit::decrease() {
	rate = bound(rate * CastorRateLimit::sigma_increase);
}

double CastorRateLimit::value() const {
	return rate;
}

double CastorRateLimit::bound(double proposed_rate) const {
	return (proposed_rate < min_rate) ? min_rate :
		   (proposed_rate > max_rate) ? max_rate :
	       /* else */				    proposed_rate;
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(CastorRateLimit)
