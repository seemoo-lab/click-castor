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
