#include <click/config.h>
#include <click/args.hh>
#include "flooding_blackhole_filter.hh"

CLICK_DECLS

int FloodingBlackholeFilter::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_or_set_p("ACTIVE", active, false)
			.complete();
}

void FloodingBlackholeFilter::push(int, Packet *p) {
	if(active)
		output(1).push(p);
	else
		output(0).push(p);
}

int FloodingBlackholeFilter::write_handler(const String &str, Element *e, void *, ErrorHandler *errh) {
	FloodingBlackholeFilter* filter = (FloodingBlackholeFilter*) e;

	bool active;
	if(Args(filter, errh).push_back_words(str)
			.read_p("ACTIVE", active)
			.complete() < 0)
		return -1;

	filter->active = active;
	return 0;
}

void FloodingBlackholeFilter::add_handlers() {
	add_write_handler("active", write_handler, 0);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(FloodingBlackholeFilter)
