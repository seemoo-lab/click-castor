#include <click/config.h>
#include <click/args.hh>
#include "forwarder_filter.hh"
#include "forwarder_list.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int ForwarderFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("NodeId", my_id)
			.complete();
}

Packet* ForwarderFilter::simple_action(Packet *p) {
	const ForwarderList& fl = reinterpret_cast<const ForwarderList&>(*p->data());
	if (fl.node == my_id || fl.node == NeighborId::make_broadcast()) {
		CastorAnno::dst_id_anno(p) = fl.node;
		return p;
	} else {
		checked_output_push(1, p);
		return 0;
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ForwarderFilter)
