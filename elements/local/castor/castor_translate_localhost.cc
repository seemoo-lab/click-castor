#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/packet.hh>
#include "castor_translate_localhost.hh"

CLICK_DECLS

CastorTranslateLocalhost::CastorTranslateLocalhost() {
}


CastorTranslateLocalhost::~CastorTranslateLocalhost() {
}


int CastorTranslateLocalhost::configure(Vector<String>& conf, ErrorHandler* errh) {
	if (Args(conf, this, errh)
		.read_mp("ADDR", myAddr)
		.complete() < 0)
			return -1;
	return 0;
}

void CastorTranslateLocalhost::push(int, Packet* p) {

	WritablePacket* q = p->uniqueify();

	q->ip_header()->ip_src.s_addr = myAddr.addr();

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorTranslateLocalhost)
