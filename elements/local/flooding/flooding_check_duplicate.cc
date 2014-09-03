#include <click/config.h>
#include <click/confparse.hh>

#include "flooding_check_duplicate.hh"

CLICK_DECLS

FloodingCheckDuplicate::FloodingCheckDuplicate() {
}

FloodingCheckDuplicate::~FloodingCheckDuplicate() {
}

int FloodingCheckDuplicate::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &crypto,
		cpEnd);
}

void FloodingCheckDuplicate::push(int, Packet *p) {

	Key id = Flooding::getId(p);
	Key dst = p->ip_header()->ip_dst.s_addr;
	HashTable<Key, Key>* ids = history.get_pointer(dst);

	if (ids && ids->get_pointer(id)) {
		output(1).push(p); // -> Duplicate
	} else {
		if (!ids) {
			history.set(dst, HashTable<Key,Key>());
			ids = history.get_pointer(dst);
		}
		ids->set(id, id);
		output(0).push(p);
	}

}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingCheckDuplicate)
