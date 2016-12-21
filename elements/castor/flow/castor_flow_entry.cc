#include <click/config.h>
#include "castor_flow_entry.hh"

CLICK_DECLS

const Hash CastorFlowEntry::default_root;

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
	return !has_tree() ? 0 : _tree->height();
}

unsigned int CastorFlowEntry::size() const {
	return !has_tree() ? 0 : _tree->size();
}

const Hash& CastorFlowEntry::fid() const {
	return !has_tree() ? default_root : _tree->root();
}

void CastorFlowEntry::set_tree(MerkleTree* tree) {
	if (_tree != NULL) {
		click_chatter("tried to reset tree!");
		return;
	}
	_tree = tree;
	expired_pkts = Bitvector((int) size());
	neighbor_acks = HashTable<NeighborId, Bitvector>(expired_pkts);
}

bool CastorFlowEntry::complete() const {
	return has_tree() && aauths != NULL && pids != NULL;
}

/* Replay protection */
bool CastorFlowEntry::is_expired_pkt(unsigned int k) const {
	if (!has_tree() || !valid_index(k))
		return false;
	return expired_pkts[k];
}

bool CastorFlowEntry::has_ack(unsigned int k, const NeighborId& from) const {
	if (!has_tree() || !valid_index(k))
		return false;
	if (neighbor_acks.count(from) == 0)
		return false;
	return neighbor_acks[from][k];
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

const Bitvector& CastorFlowEntry::get_acks(const NeighborId& from) const {
	return neighbor_acks[from];
}

bool CastorFlowEntry::valid_index(unsigned int k) const {
	return has_tree() && k < size();
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(CastorFlowEntry)
