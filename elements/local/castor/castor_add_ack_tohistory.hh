#ifndef CLICK_CASTOR_ADD_ACK_TOHISTORY_HH
#define CLICK_CASTOR_ADD_ACK_TOHISTORY_HH
#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"
#include "castor_history.hh"

CLICK_DECLS

/**
 * Adds an acknowledgement to the history.
 * The IP address entry depends on the configuration.
 * Uses either 'DST_ANNO' of the incoming packet, or a fixed IP address.
 */
class CastorAddAckToHistory: public Element {
public:
	CastorAddAckToHistory() : crypto(0), history(0) {}

	const char *class_name() const	{ return "CastorAddAckToHistory"; }
	const char *port_count() const	{ return PORTS_1_1; }
	const char *processing() const	{ return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	Crypto* crypto;
	CastorHistory* history;
};

CLICK_ENDDECLS
#endif
