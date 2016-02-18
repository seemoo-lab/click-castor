#ifndef CLICK_CASTOR_RECONSTRUCT_FLOW_HH
#define CLICK_CASTOR_RECONSTRUCT_FLOW_HH

#include <click/element.hh>
#include "castor_flow_table.hh"
#include "../crypto/crypto.hh"

CLICK_DECLS

/**
 * At the destination, we reconstruct the flow (Merkle tree)
 */
class CastorReconstructFlow : public Element {
public:
	const char *class_name() const { return "CastorReconstructFlow"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	CastorFlowTable* flowtable;
	Crypto* crypto;
};

CLICK_ENDDECLS

#endif
