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
#include "securityassociation.hh"
#include "../castor/node_id.hh"

CLICK_DECLS

typedef HashTable<NodeId, Vector<SecurityAssociation> > SAMap;

class SAManagement: public Element {
public:

	/**
	 * @param symmetricKeyLength length of generated symmetric keys
	 */
	SAManagement(size_t symmetricKeyLength = 16) : symmetricKeyLength(symmetricKeyLength), numKeys(0) {}

	const char *class_name() const	{ return "SAManagement"; }
	const char *port_count() const	{ return PORTS_0_0; }
	const char *processing() const	{ return AGNOSTIC; }

	int configure(Vector<String> &, ErrorHandler *);

	int initialize(ErrorHandler *);

	Vector<NodeId> checkSApresence(SAType t, Vector<NodeId> nodes);
	bool checkSApresence(SAType t, NodeId node);
	int addSA(SecurityAssociation, NodeId);
	void printSAs();
	const SecurityAssociation* getSA(SAType t, const NodeId& node);

private:
	int initializeSymmetricKeys(ErrorHandler*);
	int initializePublicPrivateKeys(ErrorHandler*);
	SAMap mySAs;
	NodeId myAddr; // Local nodes' IP address
	size_t symmetricKeyLength;

	// Needed for key generation
	IPAddress netAddr; // Address of the network
	size_t numKeys;
};

CLICK_ENDDECLS

#endif
