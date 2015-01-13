#ifndef CLICK_CASTOR_BEACON_GENERATOR_HH
#define CLICK_CASTOR_BEACON_GENERATOR_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/etheraddress.hh>
#include "castor.hh"

CLICK_DECLS

class CastorBeaconGenerator : public Element {
public:
	CastorBeaconGenerator();

	const char *class_name() const { return "CastorBeaconGenerator"; }
	const char *port_count() const { return PORTS_0_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);
	int initialize(ErrorHandler *) CLICK_COLD;

	void run_timer(Timer*);

private:
	Timer* timer;
	int interval;
	IPAddress myIP;
	EtherAddress myEth;
};

CLICK_ENDDECLS

#endif
