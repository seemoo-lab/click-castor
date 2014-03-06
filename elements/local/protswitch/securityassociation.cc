#include <click/config.h>
#include <click/element.hh>
#include "securityassociation.hh"
CLICK_DECLS


void SecurityAssociation::fromString(SAType type, String sa){
	myData=new unsigned char[sa.length()];
	memcpy(myData,sa.data(),sa.length());
	mySize=sa.length();
	myType=type;
}

String SecurityAssociation::toString(){
	return String(myData,mySize);
}


CLICK_ENDDECLS
ELEMENT_PROVIDES(SecurityAssociation);

