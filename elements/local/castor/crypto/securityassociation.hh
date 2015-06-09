#ifndef CLICK_SECURITY_ASSOCIATION_HH
#define CLICK_SECURITY_ASSOCIATION_HH

#include <click/element.hh>
#include <botan/symkey.h>

CLICK_DECLS

class SecurityAssociation {
public:
	enum Type {
		pubkey, privkey, endofhashchain, sharedsecret
	};

	const Type type;
	const Botan::OctetString secret;

	SecurityAssociation(Type type, const Botan::OctetString& secret) : type(type), secret(secret) {}
	String str() const;

private:
	String typeToStr(Type type) const;
};

CLICK_ENDDECLS

#endif
