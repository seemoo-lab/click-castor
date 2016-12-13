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
	CastorFlowEntry() : local(false), acked(false), aauths(NULL), pids(NULL), _tree(NULL) {}
	~CastorFlowEntry() {
		delete [] aauths;
		delete [] pids;
		delete _tree;
	}
	bool local; // am I the creator of the flow?
	bool acked; // did I receive at least one ACK for this flow?

	inline bool complete() const { return has_tree() && aauths != NULL && pids != NULL; }

	/* Merkle tree information */
	Nonce n;      // values were created from this
	Hash* aauths; // only allocated if we are an end node
	Hash* pids;   // only allocated if we are an end node
	inline unsigned int height() const { return !has_tree() ? 0 : _tree->height(); }
	inline unsigned int size() const { return !has_tree() ? 0 : _tree->size(); }
	inline const Hash& fid() const { return !has_tree() ? default_root : _tree->root(); }
	inline void set_tree(MerkleTree* tree) {
		if (_tree != NULL) {
			click_chatter("tried to reset tree!");
			return;
		}
		_tree = tree;
		expired_pkts = Bitvector((int) size());
		neighbor_acks = HashTable<NeighborId, Bitvector>(expired_pkts);
	}
	inline MerkleTree* tree() {
		return _tree;
	}
	inline bool has_tree() const { return _tree != NULL; }
	/* Replay protection */
	inline bool is_expired_pkt(unsigned int k) const {
		if (!has_tree() || !valid_index(k))
			return false;
		return expired_pkts[k];
	}
	inline bool has_ack(unsigned int k, const NeighborId& from) const {
		if (!has_tree() || !valid_index(k))
			return false;
		if (neighbor_acks.count(from) == 0)
			return false;
		return neighbor_acks[from][k];
	}
	inline void set_expired_pkt(unsigned int k) {
		if (!has_tree())
			return;
		expired_pkts[k] = true;
	}
	inline void set_ack(unsigned int k, const NeighborId& from) {
		acked = true;
		neighbor_acks[from][k] = true;
	}
	inline const Bitvector& get_acks(const NeighborId& from) const {
		return neighbor_acks[from];
	}
private:
	inline bool valid_index(unsigned int k) const {
		return has_tree() && k < size();
	}
	// Which PKTs of this flow have been expired or already ACK'd
	Bitvector expired_pkts;
	// From which neighbor have we already received which authentic ACK
	HashTable<NeighborId, Bitvector> neighbor_acks;
	MerkleTree* _tree; // (partial) Merkle tree
	static const Hash default_root;
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
private:
	Crypto* crypto;
	HashTable<FlowId, CastorFlowEntry> flows;
};

CLICK_ENDDECLS

#endif
