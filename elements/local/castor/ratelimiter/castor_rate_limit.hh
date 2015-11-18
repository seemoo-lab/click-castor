#ifndef CASTOR_RATE_LIMIT_HH
#define CASTOR_RATE_LIMIT_HH

#include <click/element.hh>

CLICK_DECLS

class CastorRateLimitTable;

/**
 * Implements TCP-inspired timeout calculation.
 */
class CastorRateLimit {
public:
	typedef unsigned int rate_t;

	CastorRateLimit() : rate(init_rate) { };

	void increase();
	void decrease();
	rate_t value() const;

	friend class CastorRateLimitTable;
private:
	rate_t rate;

	static rate_t init_rate;
	static rate_t min_rate;
	static rate_t max_rate;
	static double sigma_decrease;
	static double sigma_increase;

	inline rate_t bound(rate_t proposed_rate) const;
};

CLICK_ENDDECLS

#endif
