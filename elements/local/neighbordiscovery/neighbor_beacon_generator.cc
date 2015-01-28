#include <click/config.h>
#include <click/confparse.hh>
#include <clicknet/ether.h>
#include <click/etheraddress.hh>
#include "neighbor_beacon_generator.hh"

CLICK_DECLS

int NeighborBeaconGenerator::configure(Vector<String>& conf, ErrorHandler* errh) {
	return cp_va_kparse(conf, this, errh,
			"INTERVAL", cpkP + cpkM, cpInteger, &interval,
			"IP", cpkP + cpkM, cpIPAddress, &myIP,
			"ETH", cpkP + cpkM, cpEtherAddress, &myEth,
			cpEnd);
}

int NeighborBeaconGenerator::initialize(ErrorHandler*) {
	if (interval <= 0) {
		click_chatter("Non-positive beaconing interval. Disable beacon generator.");
		return 0;
	}

	timer = new Timer(this);
	timer->initialize(this);
	int startTime = click_random() % interval;
	timer->schedule_after_msec(startTime);

	return 0;
}

void NeighborBeaconGenerator::run_timer(Timer* timer) {
	NeighborBeacon beacon(myIP);

	WritablePacket* p = Packet::make(sizeof(click_ether), &beacon, sizeof(NeighborBeacon), 0);
	p = p->push_mac_header(sizeof(click_ether));
	p->ether_header()->ether_type = htons(ETHERTYPE_CASTOR_BEACON);
	memset(&p->ether_header()->ether_dhost, 0xff, 6); // Set broadcast address
	memcpy(&p->ether_header()->ether_shost, myEth.data(), 6);

	timer->reschedule_after_msec(interval);

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NeighborBeaconGenerator)
