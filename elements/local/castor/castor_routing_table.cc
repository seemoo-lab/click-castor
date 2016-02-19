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

HashTable<NeighborId, CastorEstimator>& CastorRoutingTable::getFlowEntry(const FlowId& flow, const SubflowId& subflow) {
	// HashTable's [] operator adds a default element if it does not exist
	return flows[flow][subflow];
}

CastorEstimator& CastorRoutingTable::getEstimator(const FlowId& flow, const SubflowId& subflow, const NeighborId& forwarder) {
	// HashTable's [] operator adds a default element if it does not exist
	return flows[flow][subflow][forwarder];
}

bool CastorRoutingTable::copyFlowEntry(const FlowId& newFlow, const FlowId& oldFlow, const SubflowId& subflow) {
	// This method might be broken for the CONTINUOUS_FLOW
	if (!flows.get(newFlow).get(subflow).empty() ||
		!flows.get(oldFlow).get(subflow).empty()) {
		return false;
	}

	flows[newFlow][subflow] = flows[oldFlow][subflow];

	return true;
}

String CastorRoutingTable::str(const FlowId& flow, const SubflowId& subflow) {
	StringAccum sa;
	sa << "Routing entry for flow " << flow.str() << " (subflow " << subflow << "):\n";
	const auto& fe = flows[flow][subflow];
	if(fe.size() == 0)
		sa << " - EMPTY \n";
	else
		for (const auto&  it : fe)
			sa << " - " << it.first << "\t" << it.second.getEstimate() << "\n";
	return String(sa.c_str());
}

void CastorRoutingTable::print(const FlowId& flow, const SubflowId& subflow) {
	click_chatter(str(flow, subflow).c_str());
}

void CastorRoutingTable::add_handlers() {
	add_read_handler("print", read_table_handler, 0);
}

String CastorRoutingTable::read_table_handler(Element *e, void *) {
	CastorRoutingTable* rt = (CastorRoutingTable*) e;
	const FlowEntry& flows = rt->flows;
	StringAccum sa;
	for (const auto& sfe : flows) {
		for (const auto& fe : sfe.second) {
			sa << rt->str(sfe.first, fe.first);
		}
	}
	return String(sa.c_str());
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRoutingTable)
