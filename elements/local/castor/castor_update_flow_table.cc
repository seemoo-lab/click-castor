#include <click/config.h>
#include <click/args.hh>
#include "castor_update_flow_table.hh"
#include "castor.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorUpdateFlowTable::configure(Vector<String> &conf, ErrorHandler *errh) {
    return Args(conf, this, errh)
    		.read_mp("FlowTable", ElementCastArg("CastorFlowTable"), flowtable)
			.read_mp("History", ElementCastArg("CastorHistory"), history)
			.complete();
}

Packet* CastorUpdateFlowTable::simple_action(Packet *p) {
	const PacketId& pid = CastorAnno::hash_anno(p);
	const FlowId& fid = history->getFlowId(pid);
	flowtable->get(fid).acked = true;
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateFlowTable)
