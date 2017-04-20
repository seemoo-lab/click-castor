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
