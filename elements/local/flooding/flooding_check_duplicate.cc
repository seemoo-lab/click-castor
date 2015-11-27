#include <click/config.h>

#include "flooding_check_duplicate.hh"

CLICK_DECLS

Packet* FloodingCheckDuplicate::simple_action(Packet *p) {
	IPAddress dst = p->ip_header()->ip_dst;
	Flooding::Id id = Flooding::id(p);
	history[dst].count(id);

	if (history[dst].count(id) > 0) {
		checked_output_push(1, p); // -> Duplicate
		return 0;
	} else {
		history[dst].set(id, id);
		return p;
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingCheckDuplicate)
