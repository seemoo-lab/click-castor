#include <click/config.h>
#include <click/args.hh>
#include "../castor.hh"
#include "castor_update_rate_limit.hh"

CLICK_DECLS

int CastorUpdateRateLimit::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("RATE_LIMITS", ElementCastArg("CastorRateLimitTable"), rate_limits)
			.read_mp("HISTORY",     ElementCastArg("CastorHistory"),        history)
			.complete();
}

Packet* CastorUpdateRateLimit::simple_action(Packet* p) {
	assert(CastorPacket::getType(p) == CastorType::ACK);

	const PacketId& pid = CastorPacket::getCastorAnno(p);

	const auto& senders = history->getPktSenders(pid);

	for (auto& sender : senders) {
		rate_limits->lookup(sender).increase();
		rate_limits->notify(sender);
	}

    return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateRateLimit)
