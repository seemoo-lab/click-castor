#include <click/config.h>
#include <click/args.hh>
#include "castor_rate_limiter.hh"
#include "../castor.hh"

CLICK_DECLS

CastorRateLimiter::RingBuffer::index_t CastorRateLimiter::RingBuffer::default_capacity;

int CastorRateLimiter::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (Args(conf, this, errh)
			.read_mp    ("RATE_LIMITS", ElementCastArg("CastorRateLimitTable"), rate_limits)
			.read_or_set("BUCKET_SIZE", CastorRateLimiter::RingBuffer::default_capacity, 3)
			.complete() < 0)
		return -1;
	rate_limits->register_listener(this);
	return 0;
}

void CastorRateLimiter::push(int, Packet* p) {
	NodeId sender = CastorPacket::src_ip_anno(p);

	verify_token_is_init(sender);

	auto& bucket = buckets[sender];
	if (!bucket.push(p)) {
		// Bucket full -> drop
		drops++;
		checked_output_push(1, p);
	}
	else {
		emit_packet(sender);
	}
}

void CastorRateLimiter::update(const NodeId& node) {
	verify_token_is_init(node);
	auto new_rate = rate_limits->lookup(node).value();
	auto& current = tokens[node];
	if (new_rate != current.rate()) {
		current.assign_adjust(new_rate, current.capacity());
		emit_packet(node); // node might now be allowed to send packet
	}
}

void CastorRateLimiter::run_timer(Timer* timer) {
	run_timer((RateTimer*) timer);
}

void CastorRateLimiter::run_timer(RateTimer* timer) {
	emit_packet(timer->node());
}

void CastorRateLimiter::emit_packet(const NodeId& node) {
	tokens[node].refill();
	auto& bucket = buckets[node];
	while (!bucket.empty() && tokens[node].contains(1)) {
		tokens[node].remove(1);
		Packet* p = bucket.pop();
		output(0).push(p);
	}
	if (bucket.empty() && tokens[node].full()) {
		tokens.erase(node);
		buckets.erase(node);
		timers.erase(node);
	} else if (!bucket.empty()) {
		verify_timer_is_init(node);
		timers[node].schedule_after(
				Timestamp::make_jiffies(tokens[node].time_until_contains(1))
		);
	} else if (!tokens[node].full()) {
		verify_timer_is_init(node);
		timers[node].schedule_after(
				Timestamp::make_jiffies(tokens[node].time_until_contains(CastorRateLimiter::RingBuffer::default_capacity))
		);
	}
}

void CastorRateLimiter::verify_timer_is_init(const NodeId& node) {
	auto& timer = timers[node];
	if (!timer.initialized()) {
		timer.set_node(node);
		timer.assign(this);
		timer.initialize(this);
	}
}

void CastorRateLimiter::verify_token_is_init(const NodeId& node) {
	if (tokens.count(node) == 0) {
		tokens.set(node, TokenBucket(
				rate_limits->lookup(node).value(),
				CastorRateLimiter::RingBuffer::default_capacity));
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRateLimiter)
