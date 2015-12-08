#ifndef CLICK_SECURITY_ASSOCIATION_HH
#define CLICK_SECURITY_ASSOCIATION_HH

#include <click/element.hh>
#include <click/vector.hh>
#include <botan/symkey.h>

CLICK_DECLS

class SecurityAssociation {
public:
	enum Type {
		pubkey, privkey, endofhashchain, sharedsecret
	};

	const Type type;
	Vector<uint8_t> secret;

	SecurityAssociation(Type type, const Vector<uint8_t>& secret) : type(type), secret(secret) { }
	String str() const;

private:
	String typeToStr(Type type) const;
};

CLICK_ENDDECLS

#endif
