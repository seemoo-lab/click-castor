#ifndef SECURITYASSOCIATION_HH
#define SECURITYASSOCIATION_HH

#include <click/element.hh>

CLICK_DECLS

enum SAType {
	SApubkey, SAprivkey, SAendofhashchain, SAsharedsecret
};

class SecurityAssociation {
public:
	const SAType myType;
	const int mySize;
	unsigned char* myData;

	SecurityAssociation(SAType, String);
	SecurityAssociation(SAType, unsigned char*, size_t);
	String toString();
};

CLICK_ENDDECLS

#endif
