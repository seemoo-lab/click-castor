#ifndef CLICK_CASTOR_RETRANSMIT_ACK_HH
#define CLICK_CASTOR_RETRANSMIT_ACK_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"

CLICK_DECLS

class CastorRetransmitAck : public Element {
public:
	CastorRetransmitAck() : history(0) {};

	const char *class_name() const	{ return "CastorRetransmitAck"; }
	const char *port_count() const	{ return PORTS_1_1; }
	const char *processing() const	{ return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	CastorHistory* history;
	NodeId myId;
};

CLICK_ENDDECLS

#endif
