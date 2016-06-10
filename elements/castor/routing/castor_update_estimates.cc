#include <click/config.h>
#include <click/args.hh>
#include "castor_update_estimates.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorUpdateEstimates::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("ROUTING_TABLE", ElementCastArg("CastorRoutingTable"), table)
			.read_mp("HISTORY", ElementCastArg("CastorHistory"), history)
			.read_or_set_p("COPY_ESTIMATORS", enableCopyEstimators, true)
			.complete();
}

Packet* CastorUpdateEstimates::simple_action(Packet *p) {
	const PacketId& pid = CastorAnno::hash_anno(p);
	const auto& fid = history->getFlowId(pid);
	const auto& routedTo = history->routedTo(pid);
	const auto& from = CastorAnno::src_id_anno(p);
	bool isFirstAck = !history->hasAck(pid);

	if (enableCopyEstimators)
		table->update(fid, history->getSource(pid), history->getDestination(pid));

	CastorEstimator& estimator = table->estimator(fid, from);
	if (routedTo == from || isFirstAck)
		estimator.increaseFirst();
	estimator.increaseAll();

	if (isFirstAck) {
		return p;
	} else { // duplicate
		checked_output_push(1, p);
		return 0;
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateEstimates)
