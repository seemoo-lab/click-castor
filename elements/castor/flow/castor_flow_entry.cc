#include <click/config.h>
#include "castor_flow_entry.hh"

CLICK_DECLS

CastorFlowEntry::CastorFlowEntry() : local(false), acked(false), aauths(NULL), pids(NULL), _tree(NULL) {}

CastorFlowEntry::~CastorFlowEntry() {
	delete[] aauths;
	delete[] pids;
	delete _tree;
}

MerkleTree* CastorFlowEntry::tree() {
	return _tree;
}

bool CastorFlowEntry::has_tree() const {
	return _tree != NULL;
}

unsigned int CastorFlowEntry::height() const {
	return _tree->height();
}

unsigned int CastorFlowEntry::size() const {
	return _tree->size();
}

const Hash& CastorFlowEntry::fid() const {
	return _tree->root();
}

void CastorFlowEntry::set_tree(MerkleTree* tree) {
	assert(!has_tree()); // tried to reset tree!
	_tree = tree;
	expired_pkts = Bitvector((int) size());
	neighbor_acks = HashTable<NeighborId, Bitvector>(expired_pkts);
}

bool CastorFlowEntry::complete() const {
	return has_tree() && aauths != NULL && pids != NULL;
}

/* Replay protection */
bool CastorFlowEntry::is_expired_pkt(unsigned int k) const {
	return valid_index(k) && expired_pkts[k];
}

bool CastorFlowEntry::has_ack(unsigned int k, const NeighborId& from) const {
	return neighbor_acks.count(from) == 0
	       && valid_index(k)
	       && neighbor_acks[from][k];
}

void CastorFlowEntry::set_expired_pkt(unsigned int k) {
	if (!has_tree())
		return;
	expired_pkts[k] = true;
}

void CastorFlowEntry::set_ack(unsigned int k, const NeighborId& from) {
	acked = true;
	neighbor_acks[from][k] = true;
}

bool CastorFlowEntry::valid_index(unsigned int k) const {
	return has_tree() && k < size();
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(CastorFlowEntry)
