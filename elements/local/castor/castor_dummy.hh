#ifndef CLICK_DUMMY_HH
#define CLICK_DUMMY_HH
#include <click/element.hh>
#include "castor.hh"
CLICK_DECLS

class Dummy : public Element {
	public:
		Dummy();
		~Dummy();
		
		const char *class_name() const	{ return "Dummy"; }
		const char *port_count() const	{ return "0/0"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);
		
		//void push(int, Packet *);
	private:
};

CLICK_ENDDECLS
#endif
