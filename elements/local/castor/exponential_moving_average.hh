#ifndef CLICK_EXPONENTIAL_MOVING_AVERAGE
#define CLICK_EXPONENTIAL_MOVING_AVERAGE

CLICK_DECLS

class ExponentialMovingAverage {
public:
	ExponentialMovingAverage (double delta) : delta(delta), alpha(0), beta(1) { };
	double get() const {
		return alpha / (alpha + beta);
	}
	void increase() {
		alpha = delta * alpha + 1;
		beta  = delta * beta;
	}
	void decrease() {
		alpha = delta * alpha;
		beta  = delta * beta  + 1;
	}
private:
	double delta; // update weight
	double alpha; // running average of packets delivered
	double beta;  // running average of packets not delivered
};

CLICK_ENDDECLS

#endif
