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
			"CastorNeighbors", cpkP+cpkM, cpElementCast, "CastorNeighbors", &neighbors,
			"BroadcastAdjust", cpkP, cpDouble, &broadcastAdjust,
			cpEnd);
}

IPAddress CastorRouteSelectorOriginal::select(const FlowId& flow, IPAddress subflow) {

	HashTable<IPAddress, CastorRoutingTable::CastorEstimator>& entry= routingtable->getFlowEntry(flow).getForwarderEntry(subflow).estimators;

	// Case 1: Routing Table is empty -> broadcast
	if (entry.size() == 0) {
		return IPAddress::make_broadcast();
	}

	// Case 2: Search for the highest estimate (break ties at random)
	Vector<IPAddress> bestEntries;
	double best = 0;
	for (HashTable<IPAddress, CastorRoutingTable::CastorEstimator>::iterator it = entry.begin(); it != entry.end(); ) {
		if(neighbors->hasNeighbor(it.key())) {
			double entryEstimate = it.value().getEstimate();
			if (entryEstimate > best) {
				bestEntries.clear();
				bestEntries.push_back(it.key());
				best = entryEstimate;
			} else if (entryEstimate >= best) {
				bestEntries.push_back(it.key());
			}
			it++;
		} else {
			// Entry timed out
			HashTable<IPAddress, CastorRoutingTable::CastorEstimator>::iterator old = it;
			it++; // save old position and increment, otherwise it might be invalid after erase
			entry.erase(old.key());
		}
	}
	if(best == 0)
		return IPAddress::make_broadcast();

	// Determine the probability of broadcasting anyway
	double broadcastProb = exp(-1 * broadcastAdjust * best);
	double rand = ((double) click_random()) / CLICK_RAND_MAX;

	if(rand <= broadcastProb) {
		// Case 2a: Broadcast
		//click_chatter("Broadcasting probability %f -> deciding to broadcast", broadcastProb);
		return IPAddress::make_broadcast();
	} else {
		// Case 2b: Unicast
		//click_chatter("Broadcasting probability %f -> deciding to unicast to %s", broadcastProb, bestEntry.nextHop.unparse().c_str());
		int randIndex = click_random() % bestEntries.size();
		return bestEntries[randIndex];
	}

}


CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRouteSelectorOriginal)
