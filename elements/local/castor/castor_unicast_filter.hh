#ifndef CLICK_CASTOR_UNICAST_FILTER_HH
#define CLICK_CASTOR_UNICAST_FILTER_HH

#include <click/element.hh>

CLICK_DECLS

/**
 * Filters unicast PKTs and pushes them to output(1).
 * Broadcast PKTs are emitted on output(0).
 */
class CastorUnicastFilter: public Element {
public:
	const char *class_name() const	{ return "CastorUnicastFilter"; }
	const char *port_count() const	{ return "1/1-2"; }
	const char *processing() const	{ return "a/ah"; }
	int configure(Vector<String>&, ErrorHandler*);

	void add_handlers();

	Packet* simple_action(Packet *);
private:
	bool active;

    static int write_handler(const String &, Element *, void *, ErrorHandler *);
};

CLICK_ENDDECLS

#endif
