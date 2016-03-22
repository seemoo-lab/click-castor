#include <click/config.h>
#include <click/args.hh>
#include <click/hashtable.hh>
#include "castor_unicast_filter.hh"
#include "castor.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorUnicastFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_or_set_p("ACTIVE", active, false)
			.complete();
}

Packet* CastorUnicastFilter::simple_action(Packet *p) {
	if (!active)
		return p;

	if (CastorAnno::dst_id_anno(p) == NeighborId::make_broadcast()) {
		return p;
	} else {
		checked_output_push(1, p);
		return 0;
	}
}

int CastorUnicastFilter::write_handler(const String &str, Element *e, void *, ErrorHandler *errh) {
	CastorUnicastFilter* filter = (CastorUnicastFilter*) e;

	bool active;
	if(Args(filter, errh).push_back_words(str)
			.read_p("ACTIVE", active)
			.complete() < 0)
		return -1;

	filter->active = active;
	return 0;
}

void CastorUnicastFilter::add_handlers() {
	add_write_handler("active", write_handler, 0);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUnicastFilter)
