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

IPAddress CastorRoutingTable::lookup(FlowId flow) {
	//Check the Routing Table for this specific flow
	Vector<RoutingEntry>* table = getRoutingTable(flow);

	// FOR DEBUG Print the Routing Table
	printRoutingTable(flow);

	//Case 1: Routing Table is empty
	if(table->size()==0){
		return IPAddress::make_broadcast();
	}

	//Search for the highest estimate
	RoutingEntry* bestEntry = &(table->front());
	for(int i=1; i<table->size(); i++){
		RoutingEntry* entry = &(table->at(i));
		if(getEstimate(entry) > getEstimate(bestEntry))
			bestEntry = entry;
	}

	// Determine the probability of still broadcasting
	double p_broad = exp(-1 * BROADCAST_ADJUST * getEstimate(bestEntry));

	//Case 2: Broadcasting
	double r = ((double) click_random()) / CLICK_RAND_MAX;

	click_chatter("Broadcasting prob: %f0.00 with random %f0.00", p_broad, r);

	if(p_broad <= r)
		return IPAddress::make_broadcast();

	//Case 3: Unicasting
	return bestEntry->nextHop;

}

void CastorRoutingTable::updateEstimates(FlowId flow, IPAddress hop,
		Operation op, Estimate est) {

	//Retrieve the  routing table
	Vector<RoutingEntry>* table = getRoutingTable(flow);

	//Retrieve the entry for our hop
	RoutingEntry* entry = getRoutingEntry(table, hop);

	// Select the values to be updated

	if(est == first && op == increase){
		entry->alpha_first 	= UPDATE_DELTA * entry->alpha_first +1;
		entry->beta_first	= UPDATE_DELTA * entry->beta_first;
	} else if (est == all && op == increase){
		entry->alpha_all 	= UPDATE_DELTA * entry->alpha_all + 1;
		entry->beta_all		= UPDATE_DELTA * entry->beta_all;
	}else if(est == first && op == decrease){
		entry->alpha_first 	= UPDATE_DELTA * entry->alpha_first;
		entry->beta_first	= entry->beta_first -1;
	} else if (est == all && op == decrease){
		entry->alpha_all 	= UPDATE_DELTA * entry->alpha_all;
		entry->beta_all		= entry->beta_all -1;
	}else
		click_chatter("Error, enum error.");
}

Vector<RoutingEntry>* CastorRoutingTable::getRoutingTable(FlowId flow) {
	for(int i=0; i<_flows.size(); i++){
		FlowEntry entry = _flows.at(i);
		if(! memcmp(entry.flow, flow, sizeof(FlowId))){
			// Found a matching entry
			return(&_flows.at(i).routes);
		}
	}

	//We have not found a matching entry in the table. Create a new one
	FlowEntry entry;
	memcpy(entry.flow, flow, sizeof(FlowId));
	entry.routes = Vector<RoutingEntry>();
	_flows.push_front(entry);
	return (&_flows.front().routes);
}

RoutingEntry* CastorRoutingTable::getRoutingEntry(Vector<RoutingEntry>* table, IPAddress hop){

	// Iterate over the Table
	for(int i=0; i<table->size(); i++){
		RoutingEntry* entry = &(table->at(i));
		if(entry->nextHop == hop){
			return entry;
		}
	}

	//If we have not found an entry, create a new one
	RoutingEntry entry;
	entry.nextHop 		= hop;
	entry.alpha_all 	= 0;
	entry.alpha_first 	= 0;
	entry.beta_all 		= 1;
	entry.beta_first 	= 1;
	table->push_back(entry);
	return &table->back();
}

void CastorRoutingTable::printRoutingTable(FlowId flow){

	StringAccum sa;
	sa << "Routing Table for Flow " << CastorPacket::hexToString(flow,sizeof(FlowId)) << ":\n";

	Vector<RoutingEntry>* table = getRoutingTable(flow);
	// Iterate over the Table
	for(int i=0; i<table->size(); i++){
		RoutingEntry* entry = &(table->at(i));
		sa << " - " << entry->nextHop << "\t" << getEstimate(entry) << "\n";
	}
	if(table->size()==0)
		sa << " -    --- empty --- \n";

	click_chatter(sa.c_str());
}

double CastorRoutingTable::getEstimate(RoutingEntry* e){
	double s_all = e->alpha_all / (e->alpha_all + e->beta_all);
	double s_first = e->alpha_first / (e->alpha_first + e->beta_first);

	return (s_all + s_first) / 2;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRoutingTable)






