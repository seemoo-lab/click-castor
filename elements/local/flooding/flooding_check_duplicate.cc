#include <click/config.h>
#include <click/confparse.hh>

#include "flooding_check_duplicate.hh"

CLICK_DECLS

void FloodingCheckDuplicate::push(int, Packet *p) {

	Flooding::Id id = Flooding::getId(p);
	IPAddress dst = p->ip_header()->ip_dst.s_addr;
	HashTable<Flooding::Id, Flooding::Id>* ids = history.get_pointer(dst);

	if (ids && ids->get_pointer(id)) {
		output(1).push(p); // -> Duplicate
	} else {
		if (!ids) {
			history.set(dst, HashTable<Flooding::Id,Flooding::Id>());
			ids = history.get_pointer(dst);
		}
		ids->set(id, id);
		output(0).push(p);
	}

}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingCheckDuplicate)
