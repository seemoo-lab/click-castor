#ifndef CLICK_FLOODING_SET_PKT_ID_HH
#define CLICK_FLOODING_SET_PKT_ID_HH

#include <click/element.hh>
#include "flooding.hh"

CLICK_DECLS

class FloodingSetPktId : public Element {
	public:
		FloodingSetPktId() : seq(0) {}

		const char *class_name() const	{ return "FloodingSetPktId"; }
		const char *port_count() const	{ return PORTS_1_1; }
		const char *processing() const	{ return PUSH; }

		void push(int, Packet *);

	private:
		Flooding::Id seq;
};

CLICK_ENDDECLS

#endif
