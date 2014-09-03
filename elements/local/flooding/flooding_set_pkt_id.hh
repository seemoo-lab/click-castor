#ifndef CLICK_FLOODING_SET_PKT_ID_HH
#define CLICK_FLOODING_SET_PKT_ID_HH

#include <click/element.hh>
#include "flooding.hh"

CLICK_DECLS

class FloodingSetPktId : public Element {
	public:
		FloodingSetPktId();
		~FloodingSetPktId();

		const char *class_name() const	{ return "FloodingSetPktId"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }

		void push(int, Packet *);

	private:
		unsigned long seq;
};

CLICK_ENDDECLS

#endif
