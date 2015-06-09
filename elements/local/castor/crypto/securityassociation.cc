#include <click/config.h>
#include <click/straccum.hh>
#include "securityassociation.hh"

CLICK_DECLS

String SecurityAssociation::str() const {
	StringAccum accum;
	accum << "SA (type " << typeToStr(type) << "): " << secret.as_string().c_str();
	return accum.take_string();
}

String SecurityAssociation::typeToStr(Type type) const {
	switch (type) {
	case privkey: return String("privkey");
	case pubkey: return String("pubkey");
	case endofhashchain: return String("endofhashchain");
	case sharedsecret: return String("sharedsecret");
	default: return String("unknown");
	}
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(SecurityAssociation);
