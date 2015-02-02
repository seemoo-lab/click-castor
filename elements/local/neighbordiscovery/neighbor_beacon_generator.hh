#ifndef CLICK_NEIGHBOR_BEACON_GENERATOR_HH
#define CLICK_NEIGHBOR_BEACON_GENERATOR_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/etheraddress.hh>
#include "neighbor_beacon.hh"

CLICK_DECLS

class NeighborBeaconGenerator : public Element {
public:
	NeighborBeaconGenerator() : timer(0), interval(0) {}

	const char *class_name() const { return "NeighborBeaconGenerator"; }
	const char *port_count() const { return PORTS_0_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);
	int initialize(ErrorHandler *);

	void run_timer(Timer*);

private:
	Timer* timer;
	unsigned int interval;
	NodeId myId;
	EtherAddress myEth;
};

CLICK_ENDDECLS

#endif
