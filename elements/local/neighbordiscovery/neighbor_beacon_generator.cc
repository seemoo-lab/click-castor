#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ether.h>
#include <click/etheraddress.hh>
#include "neighbor_beacon_generator.hh"

CLICK_DECLS

int NeighborBeaconGenerator::configure(Vector<String>& conf, ErrorHandler* errh) {
	return Args(conf, this, errh)
			.read_mp("INTERVAL", interval)
			.read_mp("ID", my_id)
			.read_or_set_p("ENABLE", enabled, true)
			.complete();
}

int NeighborBeaconGenerator::initialize(ErrorHandler* errh) {
	if (!enabled || interval == 0) {
		errh->message("Disable beacon generator.");
	} else {
		timer.assign(this);
		timer.initialize(this);
		int start_time = click_random() % interval;
		timer.schedule_after_msec(start_time);
	}
	return 0;
}

void NeighborBeaconGenerator::run_timer(Timer* timer) {
	/** Create dummy ethernet packet */
	WritablePacket* p = Packet::make(sizeof(click_ether), 0, 0, 0);
	p = p->push_mac_header(sizeof(click_ether));
	p->ether_header()->ether_type = htons(ETHERTYPE_BEACON);
	memset(&p->ether_header()->ether_dhost, 0xff, 6); // Set broadcast address
	memcpy(&p->ether_header()->ether_shost, my_id.data(), 6);

	timer->reschedule_after_msec(interval);

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NeighborBeaconGenerator)
