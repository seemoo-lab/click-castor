#include <click/config.h>
#include <click/confparse.hh>
#include <click/hashtable.hh>
#include "castor_route_selector_original.hh"

CLICK_DECLS

int CastorRouteSelectorOriginal::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &routingtable,
			"Neighbors", cpkP+cpkM, cpElementCast, "Neighbors", &neighbors,
			"BroadcastAdjust", cpkP, cpDouble, &broadcastAdjust,
			cpEnd);
}

NeighborId CastorRouteSelectorOriginal::select(const FlowId& flow, const NodeId& subflow, const Vector<NodeId>*, const PacketId &pid) {

	// Search for the neighbors with the highest estimates
	Vector<NeighborId> bestNeighbors;
	double best = findBest(routingtable->getFlowEntry(flow, subflow), bestNeighbors, pid);

	// Choose a subset (typically one or all) of the selected neighbors for PKT forwarding
	return chooseNeighbor(bestNeighbors, best, pid);
}

double CastorRouteSelectorOriginal::findBest(HashTable<NeighborId, CastorEstimator>& entry, Vector<NeighborId>& bestNeighbors, const PacketId& pid) {
	double bestEstimate = 0;
	for (HashTable<NeighborId, CastorEstimator>::iterator neighborIterator = entry.begin(); neighborIterator != entry.end();  /* increment in loop */) {
		if(neighbors->contains(neighborIterator.key())) {
			double estimate = neighborIterator.value().getEstimate();
			selectNeighbor(neighborIterator.key(), estimate, bestNeighbors, bestEstimate, pid);
			neighborIterator++;
		} else {
			// Entry timed out
			HashTable<NeighborId, CastorEstimator>::iterator old = neighborIterator;
			neighborIterator++; // save old position and increment, otherwise it might be invalid after erase
			entry.erase(old.key());
		}
	}
	return bestEstimate;
}

bool CastorRouteSelectorOriginal::selectNeighbor(const NeighborId &entry, double entryEstimate, Vector<NeighborId> &bestEntries, double &bestEstimate, const PacketId &pid)
{
	(void)pid;  // we do not make use of the packet identifier in selecting neighbors, but other selector classes may

	if (entryEstimate > bestEstimate) {
		bestEntries.clear();
		bestEntries.push_back(entry);
		bestEstimate = entryEstimate;
	} else if (entryEstimate >= bestEstimate && entryEstimate > 0) {
		bestEntries.push_back(entry);
	}
	return true;
}

NeighborId CastorRouteSelectorOriginal::chooseNeighbor(Vector<NeighborId> &bestNeighbors, double best, const PacketId &)
{
	if (best == 0)
		return selectDefault();

	// Determine the probability of broadcasting
	double broadcastProb = exp(-1 * broadcastAdjust * best);
	double rand = ((double) click_random()) / CLICK_RAND_MAX;

	if (rand <= broadcastProb /*&& neighbors->neighborCount() > 2*/) {
		// Case a: Broadcast
		//click_chatter("Broadcasting probability %f -> deciding to broadcast", broadcastProb);
		return selectDefault();
	}

	// Case b: Unicast (break ties at random)
	//click_chatter("Broadcasting probability %f -> deciding to unicast to %s", broadcastProb, bestEntry.nextHop.unparse().c_str());
	int randIndex = click_random() % bestNeighbors.size();
	return bestNeighbors[randIndex];
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRouteSelectorOriginal)
