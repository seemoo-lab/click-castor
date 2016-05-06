#include <click/config.h>
#include <click/error.hh>
#include <click/args.hh>
#include "castor_rate_limiter.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorRateLimiter::configure(Vector<String> &conf, ErrorHandler *errh) {
	int result = Args(conf, this, errh)
			.read_mp("ENABLE", enable)
			.read_p("RATE_LIMITS", ElementCastArg("CastorRateLimitTable"), rate_limits)
			.read_or_set_p("BUCKET_SIZE", capacity, 4)
			.complete();
	if (enable && rate_limits == NULL)
		return errh->error("Need rate limit table if rate limiter is enabled");
	return result;
}

int CastorRateLimiter::initialize(ErrorHandler*) {
	if (enable)
		rate_limits->register_listener(this);
	return 0;
}

void CastorRateLimiter::push(int, Packet* p) {
	if (!enable) {
		output(0).push(p);
	} else {
	const auto& sender = CastorAnno::src_id_anno(p);

	verify_entry_is_init(sender);
	auto& bucket = entries[sender].bucket;
	if (!bucket.push(p)) {
		drops++;
		checked_output_push(1, p);
	} else {
		emit_packet(sender);
	}
	}
}

void CastorRateLimiter::update(const NeighborId& node) {
	if (!enable)
		return;

	verify_entry_is_init(node);
	auto new_rate = rate_limits->lookup(node).value();
	auto& current = entries[node].tokens;
	if (new_rate != current.rate()) {
		current.assign_adjust(new_rate, capacity);
		emit_packet(node); // node might now be allowed to send packet
	}
}

void CastorRateLimiter::run_timer(Timer* timer) {
	run_timer(static_cast<RateTimer*>(timer));
}

void CastorRateLimiter::run_timer(RateTimer* timer) {
	verify_entry_is_init(timer->node());
	emit_packet(timer->node());
}

void CastorRateLimiter::emit_packet(const NeighborId& node) {
	auto& entry = entries[node];
	entry.tokens.refill();
	while (!entry.bucket.empty() && entry.tokens.contains(1)) {
		entry.tokens.remove(1);
		Packet* p = entry.bucket.pop();
		output(0).push(p);
	}
	if (entry.bucket.empty() && entry.tokens.full()) {
		entries.erase(node);
	} else if (!entry.bucket.empty()) {
		entry.timer.schedule_at_steady(
				Timestamp::now_steady() + Timestamp::make_jiffies(entry.tokens.time_until_contains(1))
		);
	} else if (!entry.tokens.full()) {
		//click_chatter("[%s] tokens not full (%d of %d): %s", Timestamp::now_steady().unparse().c_str(),
		//		entry.tokens.size(), entry.tokens.capacity(),
		//		(Timestamp::now_steady() + Timestamp::make_jiffies(entry.tokens.time_until_contains(capacity))).unparse().c_str());
		entry.timer.schedule_at_steady(
				Timestamp::now_steady() + Timestamp::make_jiffies(entry.tokens.time_until_contains(capacity))
		);
	}
}

void CastorRateLimiter::verify_entry_is_init(const NeighborId& node) {
	auto& entry = entries[node];
	if (!entry.timer.initialized()) {
		entry.timer.set_node(node);
		entry.timer.assign(this);
		entry.timer.initialize(this);
		entry.tokens.assign_adjust(rate_limits->lookup(node).value(), capacity);
		entry.bucket = RingBuffer(capacity);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRateLimiter)
