#ifndef CLICK_CASTOR_UPDATE_RATE_LIMIT_HH
#define CLICK_CASTOR_UPDATE_RATE_LIMIT_HH

#include <click/element.hh>
#include "castor_rate_limit_table.hh"
#include "../castor_history.hh"

CLICK_DECLS

class CastorUpdateRateLimit: public Element {
public:
	const char *class_name() const { return "CastorUpdateRateLimit"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	bool enable;
	CastorRateLimitTable* rate_limits;
	CastorHistory* history;
};

CLICK_ENDDECLS

#endif
