#include <click/config.h>
#include "castor_timeout.hh"

CLICK_DECLS

unsigned int CastorTimeout::init_timeout =  1000;
unsigned int CastorTimeout::min_timeout  =   100;
unsigned int CastorTimeout::max_timeout  = 60000;
// Adaptivity parameters as suggested by TCP
double CastorTimeout::alpha = 0.125; // 1/8
double CastorTimeout::beta  = 0.25;  // 1/4

void CastorTimeout::update(unsigned int new_rtt) {
	if (rtt == 0) { // first measurement
		rtt = new_rtt;
		rtt_var = 0.5 * new_rtt;
	} else { // subsequent measurement
		rtt = (1 - alpha) * rtt + alpha * new_rtt;
		unsigned int rtt_abs_diff = rtt < new_rtt ? new_rtt - rtt : rtt - new_rtt;
		rtt_var = (1 - beta) * rtt_var + beta * rtt_abs_diff;
	}
	set_new_timeout(rtt + 4 * rtt_var);
}

void CastorTimeout::packet_loss() {
	/*
	 * Do not reset measurements: might yield unstable RTT estimates
	 */
	//reset_measurements(); // this is optional according to the TCP standard
	set_new_timeout(timeout * 2);
}

unsigned int CastorTimeout::value() const {
	return timeout;
}

void CastorTimeout::reset_measurements() {
	rtt = 0;
	rtt_var = 0;
}

void CastorTimeout::set_new_timeout(unsigned int new_timeout) {
	timeout = (new_timeout < min_timeout) ? min_timeout : ((new_timeout > max_timeout) ? max_timeout : new_timeout);
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(CastorTimeout)
