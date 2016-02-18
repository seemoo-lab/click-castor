#ifndef CLICK_CASTOR_FLOW_TABLE_HH
#define CLICK_CASTOR_FLOW_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "../castor.hh"
#include "../crypto/crypto.hh"
#include "merkle_tree.hh"
#include "../../neighbordiscovery/neighbor_id.hh"

CLICK_DECLS

class CastorFlowEntry {
public:
	CastorFlowEntry() : local(false), acked(false), height(0), tree(NULL), aauths(NULL), pids(NULL) {}
	~CastorFlowEntry() {
		delete [] aauths;
		delete [] pids;
		delete tree;
	}
	bool local; // am I the creator of the flow?
	bool acked; // did I receive at least one ACK for this flow?
	NeighborId last;

	inline bool complete() const { return tree != NULL && aauths != NULL && pids != NULL; }

	/* Merkle tree information */
	unsigned int height;
	inline unsigned int size() const { return 1 << height; }
	Nonce n;      // values were created from this
	Hash* aauths; // only allocated if we are an end node
	Hash* pids;   // only allocated if we are an end node
	MerkleTree* tree; // (partial) Merkle tree
	inline const Hash& fid() const { return tree->root(); }
};

class CastorFlowTable : public Element {
public:
	const char *class_name() const { return "CastorFlowTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	/**
	 * Insert a new Merkle tree in the flow table
	 */
	bool insert(MerkleTree* tree);
	CastorFlowEntry& get(const FlowId& fid);
	MerkleTree* get(const FlowId& fid, unsigned int h);
	NeighborId& last(const FlowId& fid);
private:
	Crypto* crypto;
	HashTable<FlowId, CastorFlowEntry> flows;
};

CLICK_ENDDECLS

#endif
