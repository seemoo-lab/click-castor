/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

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
