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
	CastorSetAckNexthop();
	~CastorSetAckNexthop();
		
	const char *class_name() const { return "CastorSetAckNexthop"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }

	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	CastorHistory* _history;

};

CLICK_ENDDECLS

#endif
