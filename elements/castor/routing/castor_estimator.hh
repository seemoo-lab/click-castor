#ifndef CLICK_CASTOR_ESTIMATOR
#define CLICK_CASTOR_ESTIMATOR

#include "exponential_moving_average.hh"

CLICK_DECLS

class CastorEstimator {
public:
	CastorEstimator(double delta) : first(ExponentialMovingAverage(delta)), all(ExponentialMovingAverage(delta)) { };

	double getEstimate() const {
		return (all.get() + first.get()) / 2;
	}
	void increaseFirst() {
		first.increase();
	}
	void increaseAll() {
		all.increase();
	}
	void decreaseFrist() {
		first.decrease();
	}
	void decreaseAll() {
		all.decrease();
	}
private:
	ExponentialMovingAverage first;
	ExponentialMovingAverage all;
};

CLICK_ENDDECLS

#endif
