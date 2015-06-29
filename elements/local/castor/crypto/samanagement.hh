#ifndef CLICK_SA_MANAGEMENT_HH
#define CLICK_SA_MANAGEMENT_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "securityassociation.hh"
#include "../node_id.hh"

CLICK_DECLS

class SAManagement: public Element {
public:
	SAManagement() : symmetricKeyLength(16) {}

	const char *class_name() const	{ return "SAManagement"; }
	const char *port_count() const	{ return PORTS_0_0; }
	const char *processing() const	{ return AGNOSTIC; }

	int configure(Vector<String>&, ErrorHandler*);

	void add(const NodeId& node, const SecurityAssociation& sa);
	const SecurityAssociation* get(const NodeId& node, SecurityAssociation::Type type);
	void printall();

private:
	typedef Vector<SecurityAssociation> SAs;
	typedef HashTable<NodeId, SAs> SAMap;
	SAMap sas;

	/**
	 * XXX Warning
	 * This function should be only used for testing. It generates unique but deterministic
	 * keys for arbitrary node pairs.
	 * In a production environment, shared keys should be created using some secure key
	 * exchange protocol.
	 */
	SecurityAssociation genereateSymmetricSA(const NodeId&);
	NodeId myAddr; // Local nodes' IP address, used as seed for the KDF
	size_t symmetricKeyLength;
};

CLICK_ENDDECLS

#endif
