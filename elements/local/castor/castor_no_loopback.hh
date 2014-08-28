#ifndef CLICK_CASTOR_NO_LOOPBACK_HH
#define CLICK_CASTOR_NO_LOOPBACK_HH

#include <click/element.hh>
#include "castor.hh"

CLICK_DECLS

/**
 * Create an ACK for incoming packet, assuming that the ACK authenticator is already set as user annotation. The original packet is pushed to output 0, the ACK is pushed on output 1.
 */
class CastorNoLoopback: public Element {
public:
	CastorNoLoopback();
	~CastorNoLoopback();
		
	const char *class_name() const { return "CastorNoLoopback"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PUSH; }

	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	IPAddress myAddr;
};

CLICK_ENDDECLS

#endif
