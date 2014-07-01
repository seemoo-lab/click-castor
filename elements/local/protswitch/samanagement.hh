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
#include "../protswitch/securityassociation.hh"

CLICK_DECLS

typedef HashTable<IPAddress, Vector<SecurityAssociation> > SAMap;

class SAManagement: public Element {
public:

	/**
	 * @param symmetricKeyLength length of generated symmetric keys
	 */
	SAManagement(size_t symmetricKeyLength = 16);
	~SAManagement();

	const char *class_name() const	{ return "SAManagement"; }
	const char *port_count() const	{ return PORTS_0_0; }
	const char *processing() const	{ return AGNOSTIC; }

	int configure(Vector<String> &, ErrorHandler *);

	int initialize(ErrorHandler *);

	Vector<IPAddress> checkSApresence(SAType t, Vector<IPAddress> nodes);
	bool checkSApresence(SAType t, IPAddress node);
	int addSA(SecurityAssociation, IPAddress);
	void printSAs();
	const SecurityAssociation* getSA(SAType t, const IPAddress& node);

private:
	int initializeSymmetricKeys(ErrorHandler*);
	int initializePublicPrivateKeys(ErrorHandler*);
	SAMap mySAs;
	IPAddress myAddr; // Local nodes' IP address
	IPAddress netAddr; // Address of the network
	size_t numKeys;
	size_t symmetricKeyLength;
};

CLICK_ENDDECLS

#endif
