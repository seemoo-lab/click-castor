#include <click/config.h>
#include <click/error.hh>
#include <click/args.hh>
#include "castor_update_rate_limit.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorUpdateRateLimit::configure(Vector<String> &conf, ErrorHandler *errh) {
	rate_limits = NULL;
	history = NULL;
	int result = Args(conf, this, errh)
			.read_mp("ENABLE", enable)
			.read_p("RATE_LIMITS", ElementCastArg("CastorRateLimitTable"), rate_limits)
			.read_p("HISTORY",     ElementCastArg("CastorHistory"),        history)
			.complete();
	if (enable && (rate_limits == NULL || history == NULL))
		errh->error("If enabled, need rate limit table and history");
	return result;
}

Packet* CastorUpdateRateLimit::simple_action(Packet* p) {
	if (enable) {
		assert(CastorPacket::getType(p) == CastorType::ACK);

		const PacketId& pid = CastorAnno::hash_anno(p);

		const auto& senders = history->getPktSenders(pid);

		for (auto& sender : senders) {
			rate_limits->lookup(sender).increase();
			rate_limits->notify(sender);
		}
	}
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateRateLimit)
