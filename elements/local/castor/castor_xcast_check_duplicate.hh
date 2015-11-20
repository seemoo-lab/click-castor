#ifndef CLICK_CASTOR_XCAST_CHECK_DUPLICATE_HH
#define CLICK_CASTOR_XCAST_CHECK_DUPLICATE_HH

#include <click/element.hh>
#include "castor_history.hh"
#include "../neighbordiscovery/neighbor_id.hh"

CLICK_DECLS

class CastorXcastCheckDuplicate : public Element {
public:
	CastorXcastCheckDuplicate() : history(NULL) {}

	const char *class_name() const	{ return "CastorXcastCheckDuplicate"; }
	const char *port_count() const	{ return "1/4"; }
	const char *processing() const	{ return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	CastorHistory* history;
	NeighborId myId;
};

CLICK_ENDDECLS

#endif
