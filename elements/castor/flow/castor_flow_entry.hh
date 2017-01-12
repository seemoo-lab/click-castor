#ifndef CLICK_CASTOR_FLOW_ENTRY_HH
#define CLICK_CASTOR_FLOW_ENTRY_HH

#include <click/hashtable.hh>
#include <click/bitvector.hh>
#include "../castor.hh"
#include "../crypto/crypto.hh"
#include "merkle_tree.hh"
#include "../neighbor_id.hh"

class CastorFlowEntry {
public:
	CastorFlowEntry();
	~CastorFlowEntry();
	bool local; // am I the creator of the flow?
	bool acked; // did I receive at least one ACK for this flow?

	/* Merkle tree information */
	Nonce n;      // values were created from this
	Hash* aauths; // only allocated if we are an end node
	Hash* pids;   // only allocated if we are an end node

	unsigned int height() const;
	unsigned int size() const;
	const Hash& fid() const;
	void set_tree(MerkleTree* tree);
	MerkleTree* tree();
	bool has_tree() const;
	bool complete() const;
	/* Replay protection */
	bool is_expired_pkt(unsigned int k) const;
	bool has_ack(unsigned int k, const NeighborId& from) const;
	void set_expired_pkt(unsigned int k);
	void set_ack(unsigned int k, const NeighborId& from);

private:
	bool valid_index(unsigned int k) const;
	// Which PKTs of this flow have been expired or already ACK'd
	Bitvector expired_pkts;
	// From which neighbor have we already received which authentic ACK
	HashTable<NeighborId, Bitvector> neighbor_acks;
	MerkleTree* _tree; // (partial) Merkle tree
};

#endif //CLICK_CASTOR_FLOW_ENTRY_HH
