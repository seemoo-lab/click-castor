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

HashTable<NodeId, CastorEstimator>& CastorRoutingTable::getFlowEntry(const FlowId& flow, const SubflowId& subflow) {
	return getEntryInsertDefault(getEntryInsertDefault(flows, flow), subflow);
}

void CastorRoutingTable::updateEstimates(const FlowId& flow, NodeId subflow, NodeId neighbor, Operation op, Estimate est) {
	CastorEstimator& estimator = getEntryInsertDefault(getFlowEntry(flow, subflow), neighbor);

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

void CastorRoutingTable::printRoutingTable(const FlowId& flow, NodeId subflow) {
	StringAccum sa;
	sa << "Routing table for flow " << CastorPacket::hexToString(flow,sizeof(FlowId)) << " (" << subflow<< "):\n";

	ForwarderEntry& table = getFlowEntry(flow, subflow);
	// Iterate over the Table
	for (HashTable<NodeId, CastorEstimator>::iterator it = table.begin(); it != table.end(); it++) {
		sa << " - " << it.key() << "\t" << it.value().getEstimate() << "\n";
	}
	if(table.size()==0)
		sa << " -    --- empty --- \n";

	click_chatter(sa.c_str());
}

template <typename K, typename V>
V& CastorRoutingTable::getEntryInsertDefault(HashTable<K, V>& map, const K& key) {
	V* value = map.get_pointer(key);
	if(value == 0) {
		map.set(key, V()); // Insert default value
		value = map.get_pointer(key);
		assert(value != 0);
	}
	return *value;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRoutingTable)
