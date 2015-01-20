#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_routingtable.hh"

CLICK_DECLS

CastorRoutingTable::CastorRoutingTable() {
	// Default values from experimental setup of Castor technical paper
	updateDelta = 0.8;
}

int CastorRoutingTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"UpdateDelta", cpkP, cpDouble, &updateDelta,
			cpEnd);
}

CastorRoutingTable::FlowEntry& CastorRoutingTable::getFlowEntry(const FlowId& flow) {
	FlowEntry* entry = 0;
	if (!(entry = flows.get_pointer(flow))) {
		flows.set(flow, FlowEntry());
		entry = flows.get_pointer(flow);
	}
	assert(entry);
	return *entry;
}

void CastorRoutingTable::updateEstimates(const FlowId& flow, IPAddress subflow, IPAddress neighbor, Operation op, Estimate est) {
	CastorEstimator& estimator = getFlowEntry(flow).getForwarderEntry(subflow).getEstimator(neighbor);
	if (est == first && op == increase) {
		estimator.increaseFirst(updateDelta);
	} else if (est == all && op == increase) {
		estimator.increaseAll(updateDelta);
	} else if (est == first && op == decrease) {
		estimator.decreaseFrist(updateDelta);
	} else { // if (est == all && op == decrease)
		estimator.decreaseAll(updateDelta);
	}
}

void CastorRoutingTable::printRoutingTable(const FlowId& flow, IPAddress subflow) {
	StringAccum sa;
	sa << "Routing table for flow " << CastorPacket::hexToString(flow,sizeof(FlowId)) << " (" << subflow<< "):\n";

	ForwarderEntry& table = getFlowEntry(flow).getForwarderEntry(subflow);
	// Iterate over the Table
	for (HashTable<IPAddress, CastorEstimator>::iterator it = table.estimators.begin(); it != table.estimators.end(); it++) {
		sa << " - " << it.key() << "\t" << it.value().getEstimate() << "\n";
	}
	if(table.estimators.size()==0)
		sa << " -    --- empty --- \n";

	click_chatter(sa.c_str());
}



CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRoutingTable)
