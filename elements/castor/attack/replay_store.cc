#include <click/config.h>
#include <click/args.hh>

#include "replay_store.hh"

CLICK_DECLS

int ReplayStore::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_or_set_p("INTERVAL", interval, 500)
			.read_or_set_p("REPLAY_MAX", replays_max, 10)
			.read_or_set_p("ENABLE", enable, false)
			.complete();
}

void ReplayStore::add_pkt(const Hash& id, Packet* p) {
	if (!enable || table.count(id) > 0) {
		p->kill();
	} else {
		table[id] = new ReplayTimer(this, id, p, replays_max, interval);
	}
}

void ReplayStore::add_ack(const Hash& id, Packet* p) {
	if (!enable) {
		p->kill();
	} if (table.count(id) > 0) {
		table[id]->ack = p;
	}
}

void ReplayStore::run_timer(Timer* _timer) {
	if (!enable)
		return;

	ReplayTimer* timer = reinterpret_cast<ReplayTimer*>(_timer);

	if (timer->ack != NULL) {
		output(0).push(timer->pkt->clone());
		output(1).push(timer->ack->clone());
		timer->replays_left--;
	}

	if (timer->replays_left > 0) {
		timer->reschedule_after_msec(interval);
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

void ReplayStore::add_handlers() {
	add_write_handler("enable", write_handler, 0);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ReplayStore)
