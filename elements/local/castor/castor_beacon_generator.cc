#include <click/config.h>
#include <click/confparse.hh>
#include <clicknet/ether.h>
#include <click/etheraddress.hh>
#include "castor_beacon_generator.hh"

CLICK_DECLS

CastorBeaconGenerator::CastorBeaconGenerator() {
	timer = 0;
	interval = 0;
}

int CastorBeaconGenerator::configure(Vector<String>& conf, ErrorHandler* errh) {
	return cp_va_kparse(conf, this, errh,
			"INTERVAL", cpkP + cpkM, cpInteger, &interval,
			"IP", cpkP + cpkM, cpIPAddress, &myIP,
			"ETH", cpkP + cpkM, cpEtherAddress, &myEth,
			cpEnd);
}

int CastorBeaconGenerator::initialize(ErrorHandler*) {
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

void CastorBeaconGenerator::run_timer(Timer* timer) {
	CastorBeacon beacon;
	beacon.src = myIP;

	WritablePacket* p = Packet::make(sizeof(click_ether), &beacon, sizeof(CastorBeacon), 0);
	p = p->push_mac_header(sizeof(click_ether));

	memset(&p->ether_header()->ether_dhost, 0xff, 6);
	memcpy(&p->ether_header()->ether_shost, myEth.data(), 6);

	p->ether_header()->ether_type = htons(ETHERTYPE_CASTOR_BEACON);

	timer->reschedule_after_msec(interval);

	output(0).push(p);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorBeaconGenerator)
