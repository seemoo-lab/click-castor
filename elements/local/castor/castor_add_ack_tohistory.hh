#ifndef CLICK_CASTOR_ADD_ACK_TOHISTORY_HH
#define CLICK_CASTOR_ADD_ACK_TOHISTORY_HH
#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"

CLICK_DECLS

/**
 * Adds an acknowledgement to the history.
 * The IP address entry depends on the configuration.
 * Uses either 'DST_ANNO' of the incoming packet, or a fixed IP address.
 */
class CastorAddAckToHistory: public Element {
public:
	CastorAddAckToHistory() : history(NULL) {}

	const char *class_name() const	{ return "CastorAddAckToHistory"; }
	const char *port_count() const	{ return PORTS_1_1; }
	const char *processing() const	{ return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	CastorHistory* history;
};

CLICK_ENDDECLS
#endif
