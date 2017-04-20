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
#include "castor_flow_table.hh"

CLICK_DECLS

int CastorFlowTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	unsigned int timeout, clean_interval;

	if (Args(conf, this, errh)
			    .read_or_set("TIMEOUT", timeout, 10000)
			    .read_or_set("CLEAN", clean_interval, 1000)
			    .complete() < 0)
		return -1;

	flows = new ephemeral_map<FlowId, CastorFlowEntry>(Timestamp::make_msec(timeout), Timestamp::make_msec(clean_interval), CastorFlowEntry(), this);

	return 0;
}

CastorFlowEntry &CastorFlowTable::get(const FlowId &fid) {
	return flows->at(fid);
}

bool CastorFlowTable::has(const FlowId &fid) const {
	return flows->count(fid) > 0;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlowTable)
