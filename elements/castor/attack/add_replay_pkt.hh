#ifndef CLICK_ADD_REPLAY_PKT_HH
#define CLICK_ADD_REPLAY_PKT_HH

#include <click/element.hh>
#include "replay_store.hh"

CLICK_DECLS

class AddReplayPkt : public Element {
public:
	const char *class_name() const { return "AddReplayPkt"; }
	const char *port_count() const { return PORTS_1_1X2; }
	const char *processing() const { return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	ReplayStore* store;
};

CLICK_ENDDECLS
#endif
