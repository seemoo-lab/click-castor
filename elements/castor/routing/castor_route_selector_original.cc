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
#include <click/hashtable.hh>
#include "castor_route_selector_original.hh"

CLICK_DECLS

int CastorRouteSelectorOriginal::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("RT", ElementCastArg("CastorRoutingTable"), routingtable)
			.read_mp("NEIGHBORS", ElementCastArg("Neighbors"), neighbors)
			.read_mp("CONTINUOUS_FLOW", ElementCastArg("CastorContinuousFlowMap"), continuous_flow)
			.read_mp("TIMEOUT_TABLE", ElementCastArg("CastorTimeoutTable"), timeouttable)
			.read_or_set_p("BROADCAST_ADJUST", broadcastAdjust, 8)
			.read_or_set_p("TIEBREAKER_RTT", tiebreakerRtt, false)
			.read_or_set_p("UNICAST_THRESHOLD", unicastThreshold, 1.0)
			.read_or_set_p("RAW_RELIABILITY_FOR_BROADCAST", rawReliabilityForBroadcast, false)
			.complete();
}

void CastorRouteSelectorOriginal::copy_flow_entry(const Hash &flow, const NodeId &src, const NodeId &dst) {
	if (!routingtable->count(flow)) {
		const Hash &previous = continuous_flow->previous(src, dst);
		if (routingtable->count(previous)) {
			CastorRoutingTable::FlowEntry &previous_entry = routingtable->at(previous);
			routingtable->insert(flow, previous_entry);
		}
	}
}

NeighborId CastorRouteSelectorOriginal::select(const Hash& flow, const NodeId& src, const NodeId& dst) {
	// Optionally copy old estimators for new flow
	copy_flow_entry(flow, src, dst);
	// Search for the neighbor with the highest estimate
	Vector<NeighborId> best_candidates;
	double best_estimate = select(routingtable->at(flow), best_candidates);
	// Depending on the best estimate, we decide whether to broadcast anyway
	if (should_unicast(best_estimate))
		if (tiebreakerRtt)
			return select_smallest_rtt(flow, best_candidates);
		else
			return select_random(best_candidates);
	else
		return broadcast();
}

static int compareDouble(double a, double b) {
	return a < b ? -1 : a > b ? 1 : 0;
}

static int compareEstimatorsOriginal(const CastorEstimator& a, const CastorEstimator& b) {
	return compareDouble(a.getEstimate(), b.getEstimate());
}

static int compareEstimatorsConvergence(const CastorEstimator& a, const CastorEstimator& b) {
	int result_all = compareDouble(a.getEstimateAll(), b.getEstimateAll());
	int result_first = compareDouble(a.getEstimateFirst(), b.getEstimateFirst());
	return result_all ? result_all : result_first;
}

double CastorRouteSelectorOriginal::select(CastorRoutingTable::FlowEntry& entry, Vector<NeighborId>& best_candidates) {
	CastorEstimator null_estimate(0);
	CastorEstimator& best_estimate = null_estimate;
	for (CastorRoutingTable::FlowEntry::iterator candidate_it = entry.begin(); candidate_it != entry.end(); /* increment in loop */) {
		if (neighbors->contains(candidate_it.key())) {
			const NeighborId& candidate = candidate_it.key();
			const CastorEstimator &candidate_estimate = candidate_it.value();
			// update_candidates(candidate, candidate_estimate, best_candidates, best_estimate);
			int result = tiebreakerRtt ? compareEstimatorsConvergence(candidate_estimate, best_estimate) :
										 compareEstimatorsOriginal(candidate_estimate, best_estimate);
			if (result > 0) {
				best_candidates.clear();
				best_candidates.push_back(candidate);
				best_estimate = candidate_estimate;
			} else if (result == 0) {
				best_candidates.push_back(candidate);
			} else {
				/* nothing changed */
			}
			candidate_it++;
		} else { // This is no longer our neighbor; erase entry for this neighbor
			candidate_it = entry.erase(candidate_it);
		}
	}
	return tiebreakerRtt ? best_estimate.getEstimateAll() : best_estimate.getEstimate();
}

bool CastorRouteSelectorOriginal::should_unicast(double best_estimate) const {
	double p = best_estimate;
	if (!rawReliabilityForBroadcast)
		p = 1 - exp(-1 * broadcastAdjust * best_estimate);
	if (p >= unicastThreshold)
		return true;
	double rand = ((double) click_random()) / CLICK_RAND_MAX;
	return rand <= p;
}

const NeighborId& CastorRouteSelectorOriginal::select_random(const Vector<NeighborId>& list) const {
	int rand = click_random() % list.size();
	return list[rand];
}

const NeighborId& CastorRouteSelectorOriginal::select_smallest_rtt(const FlowId& flow, const Vector<NeighborId>& list) const {
	const NeighborId *candidate = NULL;
	Timestamp smallest_timeout;

	for (const NeighborId& neighbor : list) {
		Timestamp timeout = timeouttable->getTimeoutNoTouch(flow, neighbor).average_rtt();
		if (candidate == NULL || smallest_timeout > timeout) {
			candidate = &neighbor;
			smallest_timeout = timeout;
		}
	}

	return *candidate;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRouteSelectorOriginal)
