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

#include "replay_store.hh"

CLICK_DECLS

int ReplayStore::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_or_set_p("INTERVAL", interval, 500)
			.read_or_set_p("JITTER", jitter, 100)
			.read_or_set_p("REPLAY_MAX", replays_max, 10)
			.read_or_set_p("ENABLE", enable, false)
			.complete();
}

void ReplayStore::add_pkt(const Hash& id, Packet* p) {
	if (!enable || table.count(id) > 0) {
		p->kill();
	} else {
		table[id] = new ReplayTimer(this, id, p, replays_max, interval, jitter);
	}
}

void ReplayStore::add_ack(const Hash& id, Packet* p) {
	if (!enable) {
		p->kill();
	} if (table.count(id) > 0) {
		ReplayTimer* timer = table[id];
		timer->ack = p;
		/* now we have a complete pair, start injecting */
		timer->schedule_after_msec(timer->interval());
	}
}

void ReplayStore::run_timer(Timer* _timer) {
	if (!enable)
		return;

	ReplayTimer* timer = reinterpret_cast<ReplayTimer*>(_timer);

	output(0).push(timer->pkt->clone());
	output(1).push(timer->ack->clone());
	timer->replays_left--;

	if (timer->replays_left > 0) {
		timer->reschedule_after_msec(timer->interval());
	} else {
		table.erase(timer->id);
		delete timer;
	}
}

int ReplayStore::write_handler(const String &str, Element *e, void *, ErrorHandler *errh) {
	ReplayStore* store = (ReplayStore*) e;

	bool enable;
	if(Args(store, errh).push_back_words(str)
			.read_mp("ENABLE", enable)
			.complete() < 0)
		return -1;

	store->enable = enable;
	return 0;
}

int ReplayStore::write_clear_handler(const String &str, Element *e, void *, ErrorHandler *errh) {
    ReplayStore* store = (ReplayStore*) e;
	store->table.clear();
}

void ReplayStore::add_handlers() {
	add_write_handler("enable", write_handler, 0);
	add_write_handler("clear", write_clear_handler, 0);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ReplayStore)
