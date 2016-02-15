#ifndef CLICK_CASTOR_FLOW_TABLE_HH
#define CLICK_CASTOR_FLOW_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "castor.hh"
#include "crypto/crypto.hh"
#include "flow/merkle_tree.hh"

CLICK_DECLS

class CastorFlowTable : public Element {
public:
	const char *class_name() const { return "CastorFlowTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	MerkleTree* get(const FlowId& fid, unsigned int h);

	void add(const FlowId& fid, unsigned int h, unsigned int k, const Hash& in, const Hash fauth[], unsigned int n);
private:
	Crypto* crypto;
	HashTable<FlowId, MerkleTree*> flows;
};

CLICK_ENDDECLS

#endif
