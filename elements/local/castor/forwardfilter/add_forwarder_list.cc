#include <click/config.h>
#include "add_forwarder_list.hh"
#include "forwarder_list.hh"
#include "../castor_anno.hh"

CLICK_DECLS

Packet* AddForwarderList::simple_action(Packet* p) {
	WritablePacket* q = p->push(sizeof(ForwarderList));
	if (!q)
		return 0;
	ForwarderList ff;
	ff.num = 1;
	ff.node = CastorAnno::dst_id_anno(q);
	memcpy(q->data(), &ff, sizeof(ForwarderList));
	return q;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(AddForwarderList)
