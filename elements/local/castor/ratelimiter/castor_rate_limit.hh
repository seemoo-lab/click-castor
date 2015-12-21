#ifndef CASTOR_RATE_LIMIT_HH
#define CASTOR_RATE_LIMIT_HH

CLICK_DECLS

class CastorRateLimitTable;

class CastorRateLimit {
public:
	typedef unsigned int rate_t;

	CastorRateLimit(rate_t init, rate_t min, rate_t max, double decrease, double increase) :
		min_rate(min), max_rate(max), sigma_decrease(decrease), sigma_increase(increase) {
		rate = bound(init);
	};

	void increase();
	void decrease();
	rate_t value() const;

	friend class CastorRateLimitTable;
private:
	rate_t rate;

	rate_t min_rate;
	rate_t max_rate;
	double sigma_decrease;
	double sigma_increase;

	rate_t bound(rate_t proposed_rate) const;
};

CLICK_ENDDECLS

#endif
