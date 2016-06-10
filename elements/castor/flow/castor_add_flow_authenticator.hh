#ifndef CLICK_CASTOR_ADD_FLOW_AUTHENTICATOR_HH
#define CLICK_CASTOR_ADD_FLOW_AUTHENTICATOR_HH

#include <click/element.hh>
#include "castor_flow_table.hh"

CLICK_DECLS

/**
 * Adds a variable-sized flow authenticator to the PKT
 */
class CastorAddFlowAuthenticator : public Element {
public:
	const char *class_name() const { return "CastorAddFlowAuthenticator"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	CastorFlowTable* flowtable;
	bool force_full_auth;

	unsigned int fauth_size(unsigned int k, unsigned int fsize, const FlowId& fid, const NeighborId& neighbor) {
		const CastorFlowEntry& entry = flowtable->get(fid);
		for (unsigned int l = 0; l < fsize; ++l) {
			// [min, max[ is the range of indices for all leafs in the sibling subtree on level l
			unsigned int min = (k ^ (1 << l)) & (-1 << l);
			unsigned int max = min + (1 << l);
			// if we have received _any_ ACK in this subtree, we can return
			for (unsigned int i = min; i < max; ++i)
				if (entry.has_ack(i, neighbor))
					return l;
		}
		// did not have any ACKs
		return fsize;
	}
};

CLICK_ENDDECLS

#endif
