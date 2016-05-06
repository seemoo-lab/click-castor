#include <click/config.h>
#include "remove_forwarder_list.hh"
#include "forwarder_list.hh"

CLICK_DECLS

Packet* RemoveForwarderList::simple_action(Packet* p) {
	p->pull(sizeof(ForwarderList));
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RemoveForwarderList)
