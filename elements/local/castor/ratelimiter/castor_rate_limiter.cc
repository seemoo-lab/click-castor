#include <click/config.h>
#include <click/args.hh>
#include "castor_rate_limiter.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorRateLimiter::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("RATE_LIMITS", ElementCastArg("CastorRateLimitTable"), rate_limits)
			.read_or_set_p("BUCKET_SIZE", capacity, 3)
			.complete();
}

int CastorRateLimiter::initialize(ErrorHandler*) {
	rate_limits->register_listener(this);
	// Set 'capacity' as default for new RingBuffers
	buckets = HashTable<const NeighborId, RingBuffer>(RingBuffer(capacity));
	return 0;
}

void CastorRateLimiter::push(int, Packet* p) {
	const auto& sender = CastorAnno::src_id_anno(p);

	verify_token_is_init(sender);

	auto& bucket = buckets[sender];
	if (!bucket.push(p)) {
		drops++;
		checked_output_push(1, p);
	} else {
		emit_packet(sender);
	}
}

void CastorRateLimiter::update(const NeighborId& node) {
	verify_token_is_init(node);
	auto new_rate = rate_limits->lookup(node).value();
	auto& current = tokens[node];
	if (new_rate != current.rate()) {
		current.assign_adjust(new_rate, current.capacity());
		emit_packet(node); // node might now be allowed to send packet
	}
}

void CastorRateLimiter::run_timer(Timer* timer) {
	run_timer(static_cast<RateTimer*>(timer));
}

void CastorRateLimiter::run_timer(RateTimer* timer) {
	emit_packet(timer->node());
}

void CastorRateLimiter::emit_packet(const NeighborId& node) {
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
				Timestamp::make_jiffies(tokens[node].time_until_contains(capacity))
		);
	}
}

void CastorRateLimiter::verify_timer_is_init(const NeighborId& node) {
	auto& timer = timers[node];
	if (!timer.initialized()) {
		timer.set_node(node);
		timer.assign(this);
		timer.initialize(this);
	}
}

void CastorRateLimiter::verify_token_is_init(const NeighborId& node) {
	if (tokens.count(node) == 0) {
		tokens.set(node, TokenBucket(
				rate_limits->lookup(node).value(),
				capacity));
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRateLimiter)
