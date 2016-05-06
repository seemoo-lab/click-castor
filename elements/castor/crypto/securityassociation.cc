#include <click/config.h>
#include "securityassociation.hh"

CLICK_DECLS

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
