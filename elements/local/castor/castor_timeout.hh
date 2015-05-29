#ifndef CLICK_CASTOR_TIMEOUT_HH
#define CLICK_CASTOR_TIMEOUT_HH

#include <click/element.hh>

CLICK_DECLS

class CastorTimeoutTable;

/**
 * Implements TCP-inspired timeout calculation.
 */
class CastorTimeout {
public:
	CastorTimeout() : timeout(init_timeout), rtt(0), rtt_var(0) { };

	/**
	 * Update the timeout object with a new round-trip time sample.
	 */
	void update(unsigned int new_rtt);

	/**
	 * Indicates that an ACK was not received within the timeout interval.
	 *
	 * Doubles the current timeout window. Resets current measurements.
	 */
	void packet_loss();

	/**
	 * Get the current timeout in milliseconds.
	 */
	unsigned int value() const;

	friend class CastorTimeoutTable;
private:
	unsigned int timeout;
	unsigned int rtt;
	unsigned int rtt_var;

	static unsigned int init_timeout;
	static unsigned int min_timeout;
	static unsigned int max_timeout;
	static double alpha;
	static double beta;

	void reset_measurements();

	/*
	 * assures that min_timeout <= new_timeout <= max_timeout
	 */
	void set_new_timeout(unsigned int new_timeout);
};

CLICK_ENDDECLS

#endif
