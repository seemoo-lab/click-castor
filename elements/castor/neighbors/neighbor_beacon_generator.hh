#ifndef CLICK_NEIGHBOR_BEACON_GENERATOR_HH
#define CLICK_NEIGHBOR_BEACON_GENERATOR_HH

#include <click/element.hh>
#include <click/timer.hh>
#include "../neighbor_id.hh"

#define ETHERTYPE_BEACON 0x88B5 // 0x88B5 and 0x88B6 reserved for private experiments, so we use them

CLICK_DECLS

class NeighborBeaconGenerator : public Element {
public:
	const char *class_name() const { return "NeighborBeaconGenerator"; }
	const char *port_count() const { return PORTS_0_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);
	int initialize(ErrorHandler*);

	void run_timer(Timer*);
private:
	Timer timer;
	bool enabled;
	unsigned int interval;
	NeighborId my_id;
};

CLICK_ENDDECLS

#endif
