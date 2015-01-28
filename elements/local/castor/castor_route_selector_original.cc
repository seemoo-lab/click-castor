#include <click/config.h>
#include <click/confparse.hh>
#include <click/hashtable.hh>
#include "castor_route_selector_original.hh"

CLICK_DECLS

CastorRouteSelectorOriginal::CastorRouteSelectorOriginal() {
	// Default value from experimental setup in Castor technical paper
	broadcastAdjust = 8;
	routingtable = 0;
	neighbors = 0;
}

int CastorRouteSelectorOriginal::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &routingtable,
			"Neighbors", cpkP+cpkM, cpElementCast, "Neighbors", &neighbors,
			"BroadcastAdjust", cpkP, cpDouble, &broadcastAdjust,
			cpEnd);
}

IPAddress CastorRouteSelectorOriginal::select(const FlowId& flow, IPAddress subflow, const Vector<IPAddress>*, const PacketId &pid) {

	// Search for the highest estimate
	Vector<IPAddress> bestNeighbors;
	double best = findBest(routingtable->getFlowEntry(flow, subflow), bestNeighbors, pid);
	if (best == 0)
		return selectDefault();

	// Determine the probability of broadcasting anyway
	double broadcastProb = exp(-1 * broadcastAdjust * best);
	double rand = ((double) click_random()) / CLICK_RAND_MAX;

	if(rand <= broadcastProb) {
		// Case a: Broadcast
		//click_chatter("Broadcasting probability %f -> deciding to broadcast", broadcastProb);
		return selectDefault();
	} else {
		// Case b: Unicast (break ties at random)
		//click_chatter("Broadcasting probability %f -> deciding to unicast to %s", broadcastProb, bestEntry.nextHop.unparse().c_str());
		int randIndex = click_random() % bestNeighbors.size();
		return bestNeighbors[randIndex];
	}

}

double CastorRouteSelectorOriginal::findBest(HashTable<IPAddress, CastorEstimator>& entry, Vector<IPAddress>& bestNeighbors, const PacketId& pid) {
	double bestEstimate = 0;
	for (HashTable<IPAddress, CastorEstimator>::iterator neighborIterator = entry.begin(); neighborIterator != entry.end();  /* increment in loop */) {
		if(neighbors->hasNeighbor(neighborIterator.key())) {
			double estimate = neighborIterator.value().getEstimate();
			if (estimate > 0) {
				selectNeighbor(neighborIterator.key(), estimate, bestNeighbors, bestEstimate, pid);
			}
			neighborIterator++;
		} else {
			// Entry timed out
			HashTable<IPAddress, CastorEstimator>::iterator old = neighborIterator;
			neighborIterator++; // save old position and increment, otherwise it might be invalid after erase
			entry.erase(old.key());
		}
	}
	return bestEstimate;
}

void CastorRouteSelectorOriginal::selectNeighbor(const IPAddress &entry, double entryEstimate, Vector<IPAddress> &bestEntries, double &bestEstimate, const PacketId &pid)
{
	(void)pid;  // we do not make use of the packet identifier in selecting neighbors, but other selector classes may

	if (entryEstimate > bestEstimate) {
		bestEntries.clear();
		bestEntries.push_back(entry);
		bestEstimate = entryEstimate;
	} else if (entryEstimate >= bestEstimate) {
		bestEntries.push_back(entry);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRouteSelectorOriginal)
