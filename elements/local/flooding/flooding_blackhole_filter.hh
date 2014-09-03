#ifndef CLICK_FLOODING_BLACKHOLE_FILTER_HH
#define CLICK_FLOODING_BLACKHOLE_FILTER_HH

#include <click/element.hh>

CLICK_DECLS

class FloodingBlackholeFilter : public Element {
	public:
		FloodingBlackholeFilter();
		~FloodingBlackholeFilter();

		const char *class_name() const	{ return "FloodingBlackholeFilter"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

		void add_handlers();

		void push(int, Packet *);
	private:
		bool active;

	    static int write_handler(const String &, Element *, void *, ErrorHandler *);
};

CLICK_ENDDECLS

#endif
