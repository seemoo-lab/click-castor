#ifndef CLICK_CASTOR_NO_LOOPBACK_HH
#define CLICK_CASTOR_NO_LOOPBACK_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"

CLICK_DECLS

/**
 * Redirects an ACK that has arrived at the PKT source to output port 1, otherwise forwards it to port 0.
 */
class CastorNoLoopback: public Element {
public:
	const char *class_name() const { return "CastorNoLoopback"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PUSH; }

	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	CastorHistory *history;
	NodeId myId;
};

CLICK_ENDDECLS

#endif
