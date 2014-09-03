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

	unsigned long id = Flooding::getId(p);

	uint32_t dst = p->ip_header()->ip_dst.s_addr;
	Key key = id ^ (unsigned long) dst;

	if (history.get_pointer(key)) {
		output(1).push(p);
	} else {
		history.set(key, key);
		output(0).push(p);
	}

}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingCheckDuplicate)
