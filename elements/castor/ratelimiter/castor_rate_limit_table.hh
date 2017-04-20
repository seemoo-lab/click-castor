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

#ifndef CLICK_CASTOR_RATE_LIMIT_TABLE_HH
#define CLICK_CASTOR_RATE_LIMIT_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "../neighbor_id.hh"
#include "castor_rate_limit.hh"

CLICK_DECLS

class CastorRateLimiter;

class CastorRateLimitTable : public Element {
public:
	CastorRateLimitTable();
	virtual ~CastorRateLimitTable();

	const char *class_name() const { return "CastorRateLimitTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);
	int initialize(ErrorHandler*);

	CastorRateLimit& lookup(const NeighborId& node);
	void register_listener(CastorRateLimiter*);
	void notify(const NeighborId&) const;
private:
	unsigned int init_rate;
	unsigned int min_rate;
	unsigned int max_rate;
	double sigma_decrease;
	double sigma_increase;

	HashTable<const NeighborId, CastorRateLimit>* _table;
	CastorRateLimiter* _listener;

	mutable unsigned int warn_count;
};

CLICK_ENDDECLS

#endif
