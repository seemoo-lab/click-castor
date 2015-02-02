#ifndef CLICK_CASTOR_UNICAST_FILTER_HH
#define CLICK_CASTOR_UNICAST_FILTER_HH

#include <click/element.hh>
#include "castor.hh"

CLICK_DECLS

/**
 * Filters unicast PKTs and pushes them to output(1).
 * Broadcast PKTs are pushed to output(0).
 *
 * Can be configured to be active or inactive.
 */
class CastorUnicastFilter: public Element {
public:
	CastorUnicastFilter() : active(false) {}

	const char *class_name() const	{ return "CastorUnicastFilter"; }
	const char *port_count() const	{ return "1/2"; }
	const char *processing() const	{ return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void add_handlers();

	void push(int, Packet *);
private:
	bool active;
	NodeId myId;

    static int write_handler(const String &, Element *, void *, ErrorHandler *);
};

CLICK_ENDDECLS

#endif
