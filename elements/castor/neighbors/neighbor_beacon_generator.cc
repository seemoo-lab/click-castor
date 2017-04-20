/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

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
	/** Create ethernet packet containing sender address as ID, to avoid empty packets */
	WritablePacket* p = Packet::make(sizeof(click_ether), my_id.data(), 6, 0);
	p = p->push_mac_header(sizeof(click_ether));
	p->ether_header()->ether_type = htons(ETHERTYPE_BEACON);
	memset(&p->ether_header()->ether_dhost, 0xff, 6); // Set broadcast address
	memcpy(&p->ether_header()->ether_shost, my_id.data(), 6);

	timer->reschedule_after_msec(interval);

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(NeighborBeaconGenerator)
