#ifndef CLICK_CASTOR_XCAST_RETRANSMIT_ACK_HH
#define CLICK_CASTOR_XCAST_RETRANSMIT_ACK_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"

CLICK_DECLS

class CastorXcastRetransmitAck : public Element {
public:
	CastorXcastRetransmitAck() : history(0) {};

	const char *class_name() const	{ return "CastorXcastRetransmitAck"; }
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
