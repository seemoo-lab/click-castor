/*
 * =c
 * SAManagement
 * =s SECURE PROTOCOLS
 * =d
 *
 * This Element stores security associations in a general form. At the moment it is only accessed by the keystore element.
 *
 */
#ifndef SAMANAGEMENT_HH
#define SAMANAGEMENT_HH
#include <click/element.hh>
#include <click/hashtable.hh>
//#include <string>

#include "../protswitch/securityassociation.hh"

//#include <botan/botan.h> //also includes std::string
//#include <botan/rsa.h>
//#include <botan/pubkey.h>

CLICK_DECLS

typedef HashTable<IPAddress,Vector<SecurityAssociation> > SAMap;

class SAManagement: public Element{
	public:

		SAManagement();
		~SAManagement();

		const char *class_name() const	{ return "SAManagement"; }
		const char *port_count() const	{ return PORTS_0_0; }
		const char *processing() const	{ return AGNOSTIC; }
		SAManagement *clone() const	{ return new SAManagement; }

		int configure(Vector<String> &, ErrorHandler *);

		int initialize(ErrorHandler *);

		Vector<IPAddress> checkSApresence(SAType t,Vector<IPAddress> nodes);
		bool checkSApresence(SAType t, IPAddress node);
		int addSA(SecurityAssociation, Vector<IPAddress> nodes);
		int addSA(SecurityAssociation sa,IPAddress node);
		void printSAs();
		SecurityAssociation * getSA(SAType t, IPAddress node);

		
	private:

		//Hashmap with SAs
		SAMap mySAs;
		IPAddress myIP;

};

CLICK_ENDDECLS
#endif
