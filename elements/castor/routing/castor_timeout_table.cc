#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "castor_timeout_table.hh"

CLICK_DECLS

int CastorTimeoutTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	int ret = cp_va_kparse(conf, this, errh,
			"INIT", cpkP, cpUnsigned, &CastorTimeout::init_timeout,
			"MIN", cpkP, cpUnsigned, &CastorTimeout::min_timeout,
			"MAX", cpkP, cpUnsigned, &CastorTimeout::max_timeout,
			"ALPHA", cpkN, cpDouble, &CastorTimeout::alpha,
			"BETA", cpkN, cpDouble, &CastorTimeout::beta,
			cpEnd);
	if (CastorTimeout::init_timeout < CastorTimeout::min_timeout || CastorTimeout::init_timeout > CastorTimeout::max_timeout) {
		errh->fatal("INIT must be in the range [MIN,MAX]");
		return -1;
	}
	if (CastorTimeout::alpha < 0 || CastorTimeout::alpha > 1 || CastorTimeout::beta < 0 || CastorTimeout::beta > 1) {
		errh->fatal("ALPHA and BETA must be in the range [0,1]");
		return -1;
	}
	flows = new ephemeral_map<FlowId, ForwarderEntry>(Timestamp::make_msec(10000), Timestamp::make_msec(1000), ForwarderEntry(), this);

	return ret;
}

CastorTimeout& CastorTimeoutTable::getTimeout(const FlowId& flow, const NeighborId& forwarder) {
	return flows->at(flow)[forwarder];
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorTimeoutTable)
