#include <click/config.h>
#include <click/args.hh>
#include "castor_rate_limiter.hh"
#include "../castor.hh"

CLICK_DECLS

unsigned int CastorRateLimiter::RingBuffer::default_capacity = 3;

int CastorRateLimiter::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("NODEID", node_id)
			.read_p("BUCKET_SIZE", CastorRateLimiter::RingBuffer::default_capacity)
			.complete();
}

void CastorRateLimiter::push(int, Packet* p) {
	NodeId sender = CastorPacket::src_ip_anno(p);

	if (buckets.count(sender) == 0) {
		// TODO make initial values configurable
		tokens.set(sender, TokenBucket(1, CastorRateLimiter::RingBuffer::default_capacity));
		assert(timers.count(sender) == 0);
		RateTimer& timer = timers[sender];
		timer.set_node(sender);
		timer.assign(this);
		timer.initialize(this);
	}

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
		timers[node].schedule_after(
				Timestamp::make_jiffies(tokens[node].time_until_contains(1))
		);
	} else if (!tokens[node].full()) {
		timers[node].schedule_after(
				Timestamp::make_jiffies(tokens[node].time_until_contains(CastorRateLimiter::RingBuffer::default_capacity))
		);
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRateLimiter)
