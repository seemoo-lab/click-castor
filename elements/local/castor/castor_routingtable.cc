#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_routingtable.hh"

CLICK_DECLS

CastorRoutingTable::CastorRoutingTable() {
	_flows = Vector<FlowEntry>();
}

CastorRoutingTable::~CastorRoutingTable() {
}

int CastorRoutingTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	// Default values from experimental setup of Castor technical paper
	broadcastAdjust = 8;
	updateDelta = 0.8;

	return cp_va_kparse(conf, this, errh,
			"BroadcastAdjust", cpkP, cpDouble, &broadcastAdjust,
			"UpdateDelta", cpkP, cpDouble, &updateDelta,
			cpEnd);
}

IPAddress CastorRoutingTable::lookup(const FlowId& flow, IPAddress subflow) {
	// FOR DEBUG Print the Routing Table
	printRoutingTable(flow, subflow);

	Vector<RoutingEntry>& table = getRoutingTable(flow, subflow);

	// Case 1: Routing Table is empty -> broadcast
	if (table.size() == 0) {
		return IPAddress::make_broadcast();
	}

	// Case 2: Search for the highest estimate (break ties at random)
	// TODO: Should only include neighbors, i.e., nodes that are within transmission range (-> beaconing?)
	Vector<RoutingEntry*> bestEntries;
	bestEntries.push_back(&table[0]);
	double best = getEstimate(table[0]);
	for (int i = 1; i < table.size(); i++) {
		RoutingEntry& entry = table[i];
		double entryEstimate = getEstimate(entry);
		if (entryEstimate > best) {
			bestEntries.clear();
			bestEntries.push_back(&entry);
			best = entryEstimate;
		} else if (entryEstimate > best) {
			bestEntries.push_back(&entry);
		}
	}
	int randIndex = click_random() % bestEntries.size();
	RoutingEntry& bestEntry = *bestEntries.at(randIndex);

	// Determine the probability of broadcasting anyway
	double broadcastProb = exp(-1 * broadcastAdjust * best);
	double rand = ((double) click_random()) / CLICK_RAND_MAX;

	if(rand <= broadcastProb) {
		// Case 2a: Broadcast
		click_chatter("Broadcasting probability %f -> deciding to broadcast", broadcastProb);
		return IPAddress::make_broadcast();
	} else {
		// Case 2b: Unicast
		click_chatter("Broadcasting probability %f -> deciding to unicast to %s", broadcastProb, bestEntry.nextHop.unparse().c_str());
		return bestEntry.nextHop;
	}

}

void CastorRoutingTable::updateEstimates(const FlowId& flow, IPAddress subflow, IPAddress neighbor, Operation op, Estimate est) {

	//Retrieve the  routing table
	Vector<RoutingEntry>& table = getRoutingTable(flow, subflow);

	//Retrieve the entry for our hop
	RoutingEntry& entry = getRoutingEntry(table, neighbor);

	if (est == first && op == increase) {
		entry.alpha_first 	= updateDelta * entry.alpha_first + 1;
		entry.beta_first	= updateDelta * entry.beta_first;
	} else if (est == all && op == increase) {
		entry.alpha_all 	= updateDelta * entry.alpha_all + 1;
		entry.beta_all		= updateDelta * entry.beta_all;
	} else if (est == first && op == decrease) {
		entry.alpha_first 	= updateDelta * entry.alpha_first;
		entry.beta_first	= updateDelta * entry.beta_first + 1;
	} else if (est == all && op == decrease) {
		entry.alpha_all 	= updateDelta * entry.alpha_all;
		entry.beta_all		= updateDelta * entry.beta_all + 1;
	} else {
		click_chatter("Error, enum error.");
	}
}

Vector<CastorRoutingTable::RoutingEntry>& CastorRoutingTable::getRoutingTable(const FlowId& flow, IPAddress subflow) {
	for (int i = 0; i < _flows.size(); i++) {
		FlowEntry& entry = _flows.at(i);
		if (!memcmp(entry.flow, flow, sizeof(FlowId)) && (entry.subflow == subflow)) {
			// Found a matching entry
			return entry.routes;
		}
	}

	// We have not found a matching entry in the table. Create a new one
	FlowEntry entry(flow, subflow);
	_flows.push_back(entry); // TODO: this was push_front() before. Why?
	return _flows.back().routes;
}

CastorRoutingTable::RoutingEntry& CastorRoutingTable::getRoutingEntry(Vector<RoutingEntry>& table, IPAddress hop) {
	for(int i=0; i<table.size(); i++){
		RoutingEntry& entry = table[i];
		if(entry.nextHop == hop)
			return entry;
	}

	// We have not found an entry, create a new one
	table.push_back(RoutingEntry(hop));
	return table.back();
}

void CastorRoutingTable::printRoutingTable(const FlowId& flow, IPAddress subflow) {

	StringAccum sa;
	sa << "Routing Table for Flow " << CastorPacket::hexToString(flow,sizeof(FlowId)) << " (" << subflow<< "):\n";

	Vector<RoutingEntry>& table = getRoutingTable(flow, subflow);
	// Iterate over the Table
	for(int i=0; i<table.size(); i++){
		RoutingEntry& entry = table[i];
		sa << " - " << entry.nextHop << "\t" << getEstimate(entry) << "\n";
	}
	if(table.size()==0)
		sa << " -    --- empty --- \n";

	click_chatter(sa.c_str());
}

double CastorRoutingTable::getEstimate(const RoutingEntry& e) const {
	double s_all = e.alpha_all / (e.alpha_all + e.beta_all);
	double s_first = e.alpha_first / (e.alpha_first + e.beta_first);
	return (s_all + s_first) / 2;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRoutingTable)






