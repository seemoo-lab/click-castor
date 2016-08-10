#ifndef CLICK_CASTOR_DEBUG_AUTHENTICATE_ACK_HH
#define CLICK_CASTOR_DEBUG_AUTHENTICATE_ACK_HH

#include <click/element.hh>
#include "../routing/castor_history.hh"

CLICK_DECLS

class CastorDebugAuthenticateAck: public Element {
public:
	const char *class_name() const { return "CastorDebugAuthenticateAck"; }	
	const char *port_count() const { return "1/2"; }	
	const char *processing() const { return PUSH; }	
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet*);
private:
	CastorHistory* history;
	NeighborId myMac;

	bool already_forwarded(const PathElem* path, uint8_t path_len);
};

CLICK_ENDDECLS

#endif
