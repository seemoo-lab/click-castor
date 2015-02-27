#include <click/config.h>
#include <click/element.hh>
#include "securityassociation.hh"

CLICK_DECLS

SecurityAssociation::SecurityAssociation(SAType type, String sa) : myType(type), mySize(sa.length()) {
	myData = new unsigned char[sa.length()];
	memcpy(myData, sa.data(), sa.length());
}

SecurityAssociation::SecurityAssociation(SAType type, unsigned char* sa, size_t length) : myType(type), mySize(length) {
	myData = new unsigned char[length];
	memcpy(myData, sa, length);
}

String SecurityAssociation::toString() const {
	return String(myData, mySize);
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(SecurityAssociation);
