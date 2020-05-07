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
#include "castor_update_timeout.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorUpdateTimeout::configure(Vector<String>& conf, ErrorHandler* errh) {
    return Args(conf, this, errh)
    		.read_mp("TIMEOUT_TABLE", ElementCastArg("CastorTimeoutTable"), table)
			.read_mp("HISTORY", ElementCastArg("CastorHistory"), history)
			.read_or_set("VERBOSE", verbose, false)
			.complete();
}

Packet* CastorUpdateTimeout::simple_action(Packet* p) {
	const PacketId& pid = CastorAnno::hash_anno(p);
	const NeighborId& from = CastorAnno::src_id_anno(p);

	// Calculate new round-trip time sample
	const Timestamp& time_sent = history->getTimestamp(pid);
	const Timestamp& time_recv = p->timestamp_anno();

	Timestamp rtt = time_recv - time_sent;
	if (rtt < 0) {
		click_chatter("[CastorUpdateTimeout] sent %s, recv %s, diff %s", time_sent.unparse().c_str(), time_recv.unparse().c_str(), rtt.unparse().c_str());
		assert(false);
	}

	if (verbose)
		click_chatter("[CastorUpdateTimeout] new RTT for pid %s: %s", pid.str().c_str(), rtt.unparse().c_str());

	// Get flow's timeout object
	const FlowId& fid = history->getFlowId(pid);
	CastorTimeout& timeout = table->getTimeout(fid, from);
	// Update timeout
	timeout.update(rtt);

	/* Also update broadcast */
	NeighborId routedTo = history->routedTo(pid);
	if (routedTo != from) {
		timeout = table->getTimeout(fid, routedTo);
		timeout.update(rtt);
	}

	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateTimeout)
