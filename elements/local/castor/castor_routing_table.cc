#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include <click/error.hh>
#include "castor_routing_table.hh"

CLICK_DECLS

int CastorRoutingTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	double updateDelta;
	if (cp_va_kparse(conf, this, errh,
			"UpdateDelta", cpkP + cpkM, cpDouble, &updateDelta,
			cpEnd) < 0)
		return -1;
	if (updateDelta < 0 || updateDelta > 1) {
		errh->error("Invalid updateDelta value: %f (should be between 0 and 1)", updateDelta);
		return -1;
	}
	// Gratuitous user warnings if values seem 'impractical'
	if (updateDelta < 0.30)
		errh->warning("Possibly unwanted updateDelta value: %f (reliability estimator adaption is too fast)", updateDelta);
	if (updateDelta > 0.95)
		errh->warning("Possibly unwanted updateDelta value: %f (reliability estimator adaption is very slow)", updateDelta);
	this->updateDelta = updateDelta;
	return 0;
}

HashTable<NodeId, CastorEstimator>& CastorRoutingTable::getFlowEntry(const FlowId& flow, const SubflowId& subflow) {
	return getEntryInsertDefault(getEntryInsertDefault(flows, flow), subflow);
}

CastorEstimator& CastorRoutingTable::getEstimator(const FlowId& flow, SubflowId subflow, NodeId forwarder) {
	return getEntryInsertDefault(getFlowEntry(flow, subflow), forwarder, CastorEstimator(updateDelta));
}

bool CastorRoutingTable::copyFlowEntry(const FlowId& newFlow, const FlowId& oldFlow, NodeId subflow) {
	const SubflowEntry* se = flows.get_pointer(oldFlow);
	if (!se)
		return false;
	const ForwarderEntry* fe = se->get_pointer(subflow);
	if (!fe)
		return false;

	SubflowEntry* nse = flows.get_pointer(newFlow);
	if (!nse) {
		// We cannot copy the complete subflow entry (contains potentially multiple ForwarderEntrys)
		SubflowEntry nse;
		nse.set(subflow, *fe);
		flows.set(newFlow, nse);
		return true;
	}
	ForwarderEntry* nfe = nse->get_pointer(subflow);
	if (!nfe) {
		nse->set(subflow, *fe);
		return true;
	}
	return false; // An entry already exists, do not override!
}

void CastorRoutingTable::printRoutingTable(const FlowId& flow, NodeId subflow) {
	StringAccum sa;
	sa << "Routing table for flow " << flow.str() << " (" << subflow << "):\n";

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
V& CastorRoutingTable::getEntryInsertDefault(HashTable<K, V>& map, const K& key, const V& default_value) {
	V* value = map.get_pointer(key);
	if(value == 0) {
		map.set(key, default_value); // Insert default value
		value = map.get_pointer(key);
		assert(value != 0);
	}
	return *value;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRoutingTable)
