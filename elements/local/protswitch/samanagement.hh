#ifndef SAMANAGEMENT_HH
#define SAMANAGEMENT_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "securityassociation.hh"
#include "../castor/node_id.hh"

CLICK_DECLS

class SAManagement: public Element {
public:
	/**
	 * @param symmetricKeyLength length of generated symmetric keys
	 */
	SAManagement() : symmetricKeyLength(16) {}

	const char *class_name() const	{ return "SAManagement"; }
	const char *port_count() const	{ return PORTS_0_0; }
	const char *processing() const	{ return AGNOSTIC; }

	int configure(Vector<String> &, ErrorHandler *);

	bool checkSApresence(SAType t, const NodeId& node);
	int addSA(SecurityAssociation, const NodeId&);
	void printSAs();
	const SecurityAssociation* getSA(SAType t, const NodeId& node);

private:
	typedef Vector<SecurityAssociation> SAs;
	typedef HashTable<NodeId, SAs> SAMap;

	SecurityAssociation genereateSymmetricSA(const NodeId&);
	SAMap mySAs;
	NodeId myAddr; // Local nodes' IP address
	size_t symmetricKeyLength;
};

CLICK_ENDDECLS

#endif
