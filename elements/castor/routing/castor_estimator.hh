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
	double getEstimateAll() const {
		return all.get();
	}
	double getEstimateFirst() const {
		return first.get();
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
