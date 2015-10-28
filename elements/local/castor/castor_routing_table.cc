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

	// Initialize the flows so that it uses the new updateDelta value as default
	flows = FlowEntry(SubflowEntry(ForwarderEntry(CastorEstimator(updateDelta))));

	return 0;
}

HashTable<NodeId, CastorEstimator>& CastorRoutingTable::getFlowEntry(const FlowId& flow, const SubflowId& subflow) {
	// HashTable's [] operator adds a default element if it does not exist
	return flows[flow][subflow];
}

CastorEstimator& CastorRoutingTable::getEstimator(const FlowId& flow, SubflowId subflow, NodeId forwarder) {
	// HashTable's [] operator adds a default element if it does not exist
	return flows[flow][subflow][forwarder];
}

bool CastorRoutingTable::copyFlowEntry(const FlowId& newFlow, const FlowId& oldFlow, NodeId subflow) {
	// This method might be broken for the CONTINUOUS_FLOW
	if (!flows.get(newFlow).get(subflow).empty() ||
		!flows.get(oldFlow).get(subflow).empty()) {
		return false;
	}

	flows[newFlow][subflow] = flows[oldFlow][subflow];

	return true;
}

void CastorRoutingTable::print(const FlowId& flow, SubflowId subflow) {
	StringAccum sa;
	sa << "Routing table for flow " << flow.str() << " (" << subflow << "):\n";

	ForwarderEntry& table = flows[flow][subflow];
	// Iterate over the Table
	if(table.size()==0)
		sa << " -    --- empty --- \n";
	else
		for (ForwarderEntry::iterator it = table.begin(); it != table.end(); it++)
			sa << " - " << it.key() << "\t" << it.value().getEstimate() << "\n";

	click_chatter(sa.c_str());
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRoutingTable)
