#include <click/config.h>
#include <click/args.hh>
#include "castor_update_timeout.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorUpdateTimeout::configure(Vector<String>& conf, ErrorHandler* errh) {
    return Args(conf, this, errh)
    		.read_mp("TIMEOUT_TABLE", ElementCastArg("CastorTimeoutTable"), table)
			.read_mp("HISTORY", ElementCastArg("CastorHistory"), history)
			.read_or_set("VERBOSE", verbose, false)
			.complete();
}

Packet* CastorUpdateTimeout::simple_action(Packet* p) {
	const PacketId& pid = CastorAnno::hash_anno(p);

	// Calculate new round-trip time sample
	const Timestamp& time_sent = history->getTimestamp(pid);
	const Timestamp& time_recv = p->timestamp_anno();
	assert(time_recv > time_sent);
	Timestamp diff = time_recv - time_sent;
	unsigned int new_rtt = diff.sec() * 1000 + diff.msec();

	if (verbose)
		click_chatter("[CastorUpdateTimeout] new RTT for pid %s: %u usec", pid.str().c_str(), diff.sec() * 1000000 + diff.usec());

	// Get flow's timeout object
	const FlowId& fid = history->getFlowId(pid);
	NodeId subfid = history->getDestination(pid);
	NeighborId routedTo = history->routedTo(pid);
	CastorTimeout& timeout = table->getTimeout(fid, subfid, routedTo);

	// Update timeout
	timeout.update(new_rtt);

	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateTimeout)
