#include <click/config.h>
#include <click/args.hh>

#include "replay_store.hh"

CLICK_DECLS

int ReplayStore::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_or_set_p("TIMEOUT", timeout, 500)
			.read_or_set_p("REPLAY_MAX", replays_max, 10)
			.complete();
}

void ReplayStore::add_pkt(const Hash& id, Packet* p) {
	if (table.count(id) > 0)
		return;
	table[id] = new ReplayTimer(this, id, p, replays_max, timeout);
}

void ReplayStore::add_ack(const Hash& id, Packet* p) {
	if (table.count(id) > 0)
		table[id]->ack = p;
}

void ReplayStore::run_timer(Timer* _timer) {
	ReplayTimer* timer = reinterpret_cast<ReplayTimer*>(_timer);

	if (timer->ack != NULL) {
		output(0).push(timer->pkt->clone());
		output(1).push(timer->ack->clone());
		timer->replays_left--;
	}

	if (timer->replays_left > 0) {
		timer->reschedule_after_msec(timeout);
	} else {
		table.erase(timer->id);
		delete timer;
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ReplayStore)
