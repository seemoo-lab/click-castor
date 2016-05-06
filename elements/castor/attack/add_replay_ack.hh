#ifndef CLICK_ADD_REPLAY_ACK_HH
#define CLICK_ADD_REPLAY_ACK_HH

#include <click/element.hh>
#include "replay_store.hh"

CLICK_DECLS

class AddReplayAck : public Element {
public:
	const char *class_name() const { return "AddReplayAck"; }
	const char *port_count() const { return PORTS_1_1X2; }
	const char *processing() const { return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	ReplayStore* store;
};

CLICK_ENDDECLS
#endif
