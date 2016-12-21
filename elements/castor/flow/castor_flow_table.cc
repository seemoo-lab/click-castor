#include <click/config.h>
#include <click/args.hh>
#include "castor_flow_table.hh"

CLICK_DECLS

int CastorFlowTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	unsigned int timeout, clean_interval;

	if (Args(conf, this, errh)
			    .read_or_set("TIMEOUT", timeout, 10000)
			    .read_or_set("CLEAN", clean_interval, 1000)
			    .complete() < 0)
		return -1;

	flows = new ephemeral_map<FlowId, CastorFlowEntry>(Timestamp::make_msec(timeout), Timestamp::make_msec(clean_interval), CastorFlowEntry(), this);

	return 0;
}

CastorFlowEntry &CastorFlowTable::get(const FlowId &fid) {
	return flows->at(fid);
}

bool CastorFlowTable::has(const FlowId &fid) const {
	return flows->count(fid) > 0;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlowTable)
