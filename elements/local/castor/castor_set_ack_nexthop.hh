#ifndef CLICK_CASTOR_SET_ACK_NEXTHOP_HH
#define CLICK_CASTOR_SET_ACK_NEXTHOP_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"

CLICK_DECLS

/**
 * Create an ACK for incoming packet, assuming that the ACK authenticator is already set as user annotation. The original packet is pushed to output 0, the ACK is pushed on output 1.
 */
class CastorSetAckNexthop: public Element {
public:
	const char *class_name() const { return "CastorSetAckNexthop"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PUSH; }

	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	CastorHistory* history;
	bool promisc;

};

CLICK_ENDDECLS

#endif
