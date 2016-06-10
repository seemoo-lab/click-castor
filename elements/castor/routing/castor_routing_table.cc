#include <click/config.h>
#include <click/args.hh>
#include <click/straccum.hh>
#include <click/error.hh>
#include "castor_routing_table.hh"

CLICK_DECLS

int CastorRoutingTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	double updateDelta;
	if (Args(conf, this, errh)
			.read_mp("UpdateDelta", updateDelta)
			.complete() < 0)
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
	flows = HashTable<FlowId, FlowEntry>(FlowEntry(NeighborEntry(updateDelta)));

	return 0;
}

HashTable<NeighborId, CastorRoutingTable::NeighborEntry>& CastorRoutingTable::entry(const Hash& flow) {
	// HashTable's [] operator adds a default element if it does not exist
	return flows[flow];
}

CastorEstimator& CastorRoutingTable::estimator(const Hash& flow, const NeighborId& forwarder) {
	// HashTable's [] operator adds a default element if it does not exist
	return flows[flow][forwarder].estimator;
}

HashTable<NeighborId, CastorRoutingTable::NeighborEntry>& CastorRoutingTable::entry_copy(const Hash& flow, const NodeId& src, const NodeId& dst) {
	Pair<NodeId,NodeId> pair(src, dst);
	if (flows.count(flow) > 0) {
		/* do nothing */;
	} else if (srcdstmap.count(pair) > 0) {
		flows[flow] = flows[srcdstmap[pair]];
	} else if (dstmap.count(dst) > 0) {
		flows[flow] = flows[dstmap[dst]];
	}
	return flows[flow];
}

void CastorRoutingTable::update(const Hash& flow, const NodeId& src, const NodeId& dst) {
	Pair<NodeId,NodeId> pair(src, dst);
	srcdstmap[pair] = flow;
	dstmap[dst] = flow;
}

String CastorRoutingTable::unparse(const Hash& flow) const {
	StringAccum sa;
	sa << "Routing entry for flow " << flow.str() << ":\n";
	const auto& fe = flows[flow];
	if(fe.size() == 0)
		sa << " - EMPTY \n";
	else
		for (const auto&  it : fe)
			sa << " - " << it.first << "\t" << it.second.estimator.getEstimate() << "\n";
	return String(sa.c_str());
}

void CastorRoutingTable::print(const Hash& flow) const {
	click_chatter(unparse(flow).c_str());
}

void CastorRoutingTable::add_handlers() {
	add_read_handler("print", read_table_handler, 0);
}

String CastorRoutingTable::read_table_handler(Element *e, void *) {
	CastorRoutingTable* rt = (CastorRoutingTable*) e;
	StringAccum sa;
	for (const auto& fe : rt->flows)
		sa << rt->unparse(fe.first);
	return String(sa.c_str());
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRoutingTable)
