#ifndef CLICK_CASTOR_CREATE_ACK_HH
#define CLICK_CASTOR_CREATE_ACK_HH

#include <click/element.hh>
#include "castor.hh"

CLICK_DECLS

/**
 * Create an ACK for incoming packet, assuming that the ACK authenticator is already decrypted. The original packet is pushed to output 0, the ACK is pushed on output 1.
 */
class CastorCreateAck: public Element {
public:
	CastorCreateAck();
	~CastorCreateAck();

	const char *class_name() const { return "CastorCreateAck"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PUSH; }

	void push(int, Packet *);
};

CLICK_ENDDECLS

#endif
