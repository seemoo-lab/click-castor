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

IPAddress CastorRouteSelectorOriginal::select(const FlowId& flow, IPAddress subflow, const PacketId &pid) {

	HashTable<IPAddress, CastorRoutingTable::CastorEstimator>& entry= routingtable->getFlowEntry(flow).getForwarderEntry(subflow).estimators;

	// Search for the highest estimate
	Vector<IPAddress> bestEntries;
	double best = 0;
	bool foundNeighbors = false;
	for (HashTable<IPAddress, CastorRoutingTable::CastorEstimator>::iterator it = entry.begin(); it != entry.end(); ) {
		if(neighbors->hasNeighbor(it.key())) {
			selectNeighbor(it.key(), it.value().getEstimate(), bestEntries, best, pid);
			foundNeighbors = true;
			it++;
		} else {
			// Entry timed out
			HashTable<IPAddress, CastorRoutingTable::CastorEstimator>::iterator old = it;
			it++; // save old position and increment, otherwise it might be invalid after erase
			entry.erase(old.key());
		}
	}

	// Return empty address if we have found neighbors but still haven't selected any
	// Return broadcast address if we could not find any neighbors
	if (bestEntries.size() == 0)
		return foundNeighbors ? IPAddress() : IPAddress::make_broadcast();

	// Determine the probability of broadcasting anyway
	double broadcastProb = exp(-1 * broadcastAdjust * best);
	double rand = ((double) click_random()) / CLICK_RAND_MAX;

	if(rand <= broadcastProb) {
		// Case a: Broadcast
		//click_chatter("Broadcasting probability %f -> deciding to broadcast", broadcastProb);
		return IPAddress::make_broadcast();
	} else {
		// Case b: Unicast (break ties at random)
		//click_chatter("Broadcasting probability %f -> deciding to unicast to %s", broadcastProb, bestEntry.nextHop.unparse().c_str());
		int randIndex = click_random() % bestEntries.size();
		return bestEntries[randIndex];
	}

}


CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRouteSelectorOriginal)
