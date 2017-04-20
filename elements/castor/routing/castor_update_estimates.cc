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
#include "castor_update_estimates.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorUpdateEstimates::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("ROUTING_TABLE", ElementCastArg("CastorRoutingTable"), table)
			.read_mp("CONTINUOUS_FLOW", ElementCastArg("CastorContinuousFlowMap"), continuous_flow)
			.read_mp("HISTORY", ElementCastArg("CastorHistory"), history)
			.read_or_set_p("COPY_ESTIMATORS", enableCopyEstimators, true)
			.complete();
}

Packet* CastorUpdateEstimates::simple_action(Packet *p) {
	const PacketId& pid = CastorAnno::hash_anno(p);
	const auto& fid = history->getFlowId(pid);
	const auto& routedTo = history->routedTo(pid);
	const auto& from = CastorAnno::src_id_anno(p);
	bool isFirstAck = !history->hasAck(pid);

	if (enableCopyEstimators)
		continuous_flow->update(fid, history->getSource(pid), history->getDestination(pid));

	CastorEstimator& estimator = table->at(fid)[from];
	if (routedTo == from || isFirstAck)
		estimator.increaseFirst();
	estimator.increaseAll();

	if (isFirstAck) {
		return p;
	} else { // duplicate
		checked_output_push(1, p);
		return 0;
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateEstimates)
