#ifndef SECURITYASSOCIATION_HH
#define SECURITYASSOCIATION_HH
#include <click/element.hh>
CLICK_DECLS

enum SAType{SApubkey,SAprivkey,SAendofhashchain,SAsharedsecret};

class SecurityAssociation{
public:
	SAType myType;
	int mySize;
	unsigned char * myData;

public:
	void fromString(SAType t,String sa);
	String toString();
};

CLICK_ENDDECLS
#endif
