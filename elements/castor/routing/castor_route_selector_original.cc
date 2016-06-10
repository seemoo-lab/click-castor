#include <click/config.h>
#include <click/args.hh>
#include <click/hashtable.hh>
#include "castor_route_selector_original.hh"

CLICK_DECLS

int CastorRouteSelectorOriginal::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("RT", ElementCastArg("CastorRoutingTable"), routingtable)
			.read_mp("NEIGHBORS", ElementCastArg("Neighbors"), neighbors)
			.read_or_set_p("BROADCAST_ADJUST", broadcastAdjust, 8)
			.complete();
}

NeighborId CastorRouteSelectorOriginal::select(const Hash& flow, const NodeId& src, const NodeId& dst) {
	// Search for the neighbor with the highest estimate
	Vector<NeighborId> best_candidates;
	double best_estimate = select(routingtable->entry_copy(flow, src, dst), best_candidates);
	// Depending on the best estimate, we decide whether to broadcast anyway
	if (should_broadcast(best_estimate))
		return broadcast();
	else
		return select_random(best_candidates);
}

double CastorRouteSelectorOriginal::select(CastorRoutingTable::FlowEntry& entry, Vector<NeighborId>& best_candidates) {
	double best_estimate = 0;
	for (CastorRoutingTable::FlowEntry::iterator candidate_it = entry.begin(); candidate_it != entry.end(); /* increment in loop */) {
		if (neighbors->contains(candidate_it.key())) {
			const NeighborId& candidate = candidate_it.key();
			double candidate_estimate = candidate_it.value().estimator.getEstimate();
			update_candidates(candidate, candidate_estimate, best_candidates, best_estimate);
			candidate_it++;
		} else { // This is no longer our neighbor; erase entry for this neighbor
			candidate_it = entry.erase(candidate_it);
		}
	}
	return best_estimate;
}

void CastorRouteSelectorOriginal::update_candidates(const NeighborId& candidate, double cansidate_estimate,
		Vector<NeighborId>& best_candidates, double& best_estimate) const {
	if (cansidate_estimate > best_estimate) {
		best_candidates.clear();
		best_candidates.push_back(candidate);
		best_estimate = cansidate_estimate;
	} else if (cansidate_estimate == best_estimate && cansidate_estimate > 0) {
		best_candidates.push_back(candidate);
	} else {
		/* nothing changed */
	}
}

bool CastorRouteSelectorOriginal::should_broadcast(double best_estimate) const {
	double p = exp(-1 * broadcastAdjust * best_estimate);
	double rand = ((double) click_random()) / CLICK_RAND_MAX;
	return rand <= p;
}

const NeighborId& CastorRouteSelectorOriginal::select_random(const Vector<NeighborId>& list) const {
	assert(list.size() > 0);
	int rand = click_random() % list.size();
	return list[rand];
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRouteSelectorOriginal)
