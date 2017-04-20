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
#include <click/args.hh>
#include <click/error.hh>
#include "castor_rate_limit_table.hh"
#include "castor_rate_limiter.hh"

CLICK_DECLS

CastorRateLimitTable::CastorRateLimitTable() : _table(NULL), _listener(NULL), warn_count(0) {}
CastorRateLimitTable::~CastorRateLimitTable() {
	delete _table;
}

int CastorRateLimitTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (Args(conf, this, errh)
		.read_or_set("MIN",  min_rate,    1)
		.read_or_set("MAX",  max_rate,  100)
		.read_or_set("INIT", init_rate,   1)
		.read_or_set("INC",  sigma_increase, 2.0)
		.read_or_set("DEC",  sigma_decrease, 0.5)
		.complete() < 0)
		return -1;
	if (init_rate < min_rate ||	init_rate > max_rate)
		return errh->error("INIT: out of bounds: %u", init_rate);
	return 0;
}

int CastorRateLimitTable::initialize(ErrorHandler*) {
	_table = new HashTable<const NeighborId, CastorRateLimit>(CastorRateLimit(init_rate, min_rate, max_rate, sigma_decrease, sigma_increase));
	return 0;
}

CastorRateLimit& CastorRateLimitTable::lookup(const NeighborId& node) {
	return (*_table)[node];
}

void CastorRateLimitTable::register_listener(CastorRateLimiter* element) {
	_listener = element;
}

void CastorRateLimitTable::notify(const NeighborId& node) const {
	if (_listener)
		_listener->update(node);
	//else if (warn_count++ < 5)
	//	click_chatter("CastorRateLimitTable::notify() called but no listener set.");
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRateLimitTable)
