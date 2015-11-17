#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "castor_rate_limit_table.hh"

CLICK_DECLS

int CastorRateLimitTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	int ret = cp_va_kparse(conf, this, errh,
			"INIT", cpkN, cpDouble, &CastorRateLimit::init_rate,
			"MIN", cpkN, cpDouble, &CastorRateLimit::min_rate,
			"MAX", cpkN, cpDouble, &CastorRateLimit::max_rate,
			"SIGMA_INC", cpkN, cpDouble, &CastorRateLimit::sigma_increase,
			"SIGMA_DEC", cpkN, cpDouble, &CastorRateLimit::sigma_decrease,
			cpEnd);
	if (CastorRateLimit::init_rate < CastorRateLimit::min_rate || CastorRateLimit::init_rate > CastorRateLimit::max_rate) {
		errh->fatal("INIT must be in the range [MIN,MAX]");
		return -1;
	}
	if (CastorRateLimit::sigma_increase < 0 || CastorRateLimit::sigma_increase > 1 || CastorRateLimit::sigma_decrease < 0 || CastorRateLimit::sigma_decrease > 1) {
		errh->fatal("SIGMA_INC and SIGMA_DEC must be in the range [0,1]");
		return -1;
	}
	return ret;
}

CastorRateLimit& CastorRateLimitTable::lookup(const NodeId& node) {
	return _table[node];
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRateLimitTable)
