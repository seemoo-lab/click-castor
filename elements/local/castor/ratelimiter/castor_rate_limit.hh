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
	CastorRateLimit() : rate(init_rate) { };

	void increase();
	void decrease();
	double value() const;

	friend class CastorRateLimitTable;
private:
	double rate;

	static double init_rate;
	static double min_rate;
	static double max_rate;
	static double sigma_decrease;
	static double sigma_increase;

	inline double bound(double proposed_rate) const;
};

CLICK_ENDDECLS

#endif
