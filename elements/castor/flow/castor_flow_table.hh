#ifndef CLICK_CASTOR_FLOW_TABLE_HH
#define CLICK_CASTOR_FLOW_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/bitvector.hh>
#include "../castor.hh"
#include "../crypto/crypto.hh"
#include "merkle_tree.hh"
#include "../neighbor_id.hh"

CLICK_DECLS

class CastorFlowEntry {
public:
	CastorFlowEntry() : local(false), acked(false), tree(NULL), aauths(NULL), pids(NULL) {}
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
	Nonce n;      // values were created from this
	Hash* aauths; // only allocated if we are an end node
	Hash* pids;   // only allocated if we are an end node
	MerkleTree* tree; // (partial) Merkle tree
	inline unsigned int height() const { return tree->height(); }
	inline unsigned int size() const { return tree->size(); }
	inline const Hash& fid() const { return tree->root(); }

	/* Replay protection */
	inline bool has_ack(unsigned int k) {
		if (tree == NULL || acks.size() == 0)
			return false;
		return acks[k];
	}
	inline bool has_ack(unsigned int k, NeighborId from) {
		if (!has_ack(k))
			return false;
		if (neighbor_acks.count(from) == 0)
			return false;
		return neighbor_acks[from][k];
	}
	inline void set_ack(unsigned int k, NeighborId from) {
		if (tree == NULL) {
			click_chatter("no tree set, but tried to set ACK");
			return;
		}
		if (acks.size() != tree->size()) {
			// Set bit vector size to the size of the tree
			acks = Bitvector((int) tree->size());
			// Flow size will not change from now,
			// so initialize neighbor_acks with appropriate value
			neighbor_acks = HashTable<NeighborId, Bitvector>(acks);
		}
		acks[k] = true;
		neighbor_acks[from][k] = true;
	}
private:
	// Which (authenticated) ACKs have we received so far ...
	Bitvector acks;
	// ... and from which neighbor
	HashTable<NeighborId, Bitvector> neighbor_acks;
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
