#include <click/config.h>
#include <click/args.hh>
#include <click/straccum.hh>
#include <click/error.hh>
#include "castor_routing_table.hh"

#ifndef MAX
  #define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

CLICK_DECLS

int CastorRoutingTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	unsigned int timeout, clean_interval;

	if (Args(conf, this, errh)
			.read_mp("UpdateDelta", updateDelta)
			.read_or_set("TIMEOUT", timeout, 10000)
			.read_or_set("CLEAN", clean_interval, 1000)
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

	default_entry = FlowEntry(CastorEstimator(updateDelta));

	flows = new ephemeral_map<FlowId, FlowEntry>(Timestamp::make_msec(timeout), Timestamp::make_msec(clean_interval), default_entry, this);

	return 0;
}

CastorRoutingTable::size_type CastorRoutingTable::count(const Hash &flow) const {
	return flows->count(flow);
}

void CastorRoutingTable::insert(const Hash &flow, const CastorRoutingTable::FlowEntry &entry) {
	(void) flows->at_or_default(flow, entry);
}

String CastorRoutingTable::unparse(const Hash& flow) const {
	StringAccum sa;
	sa << "Flow " << flow.str() << ":\n";
	const auto& fe = flows->at(flow);
	if(fe.size() == 0)
		sa << " - EMPTY \n";
	else
		for (const auto&  it : fe)
			sa << " - " << it.first << "\t" << it.second.getEstimate() << "\n";
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
	sa << "Total number of entries: " << rt->flows->size() << "\n";
	for (const auto& fe : rt->flows->map())
		sa << rt->unparse(fe.first);
	return String(sa.c_str());
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRoutingTable)
