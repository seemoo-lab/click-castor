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
#include "castor_timeout_table.hh"

CLICK_DECLS

int CastorTimeoutTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	int result = Args(conf, this, errh)
		.read_p("INIT", CastorTimeout::init_timeout)
		.read_p("MIN", CastorTimeout::min_timeout)
		.read_p("MAX", CastorTimeout::max_timeout)
		.read("ALPHA", CastorTimeout::alpha)
		.read("BETA", CastorTimeout::beta)
		.complete();
	if (result)
		return result;
	if (CastorTimeout::init_timeout < CastorTimeout::min_timeout || CastorTimeout::init_timeout > CastorTimeout::max_timeout) {
		errh->fatal("INIT must be in the range [MIN,MAX]");
		return -1;
	}
	if (CastorTimeout::alpha < 0 || CastorTimeout::alpha > 1 || CastorTimeout::beta < 0 || CastorTimeout::beta > 1) {
		errh->fatal("ALPHA and BETA must be in the range [0,1]");
		return -1;
	}
	flows = new ephemeral_map<FlowId, ForwarderEntry>(Timestamp::make_msec(10000), Timestamp::make_msec(1000), ForwarderEntry(), this);

	return 0;
}

CastorTimeout& CastorTimeoutTable::getTimeout(const FlowId& flow, const NeighborId& forwarder) {
	return flows->at(flow)[forwarder];
}

const CastorTimeout& CastorTimeoutTable::getTimeoutNoTouch(const FlowId& flow, const NeighborId& forwarder) const {
	return flows->at_notouch(flow)[forwarder];
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorTimeoutTable)
