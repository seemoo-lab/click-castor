#include <click/config.h>
#include "castor_merkle_flow.hh"

CLICK_DECLS

CastorMerkleFlow::CastorMerkleFlow(size_t size, const Crypto* crypto) : size(size), height(MerkleTree::log2(size)), pos(0) {
	aauths = new Hash[size];
	pids = new Hash[size];
	for (int i = 0; i < size; i++) {
		crypto->random(aauths[i]);
		crypto->hash(pids[i], aauths[i]);
	}
	tree = new MerkleTree(pids, size, *crypto);
}

CastorMerkleFlow::~CastorMerkleFlow() {
	delete [] aauths;
	delete [] pids;
	delete tree;
}

PacketLabel CastorMerkleFlow::freshLabel() {
	if (!isAlive())
		return PacketLabel();
	PacketLabel label;
	label.num = pos;
	label.size = height;
	label.fid = tree->root();
	label.pid = pids[pos];
	label.fauth = new Hash[height];
	// Set flow authenticator
	tree->path_to_root(pos, label.fauth);
	label.aauth = aauths[pos];

	pos++;

	return label;
}

bool CastorMerkleFlow::isAlive() const {
	return pos < size;
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
ELEMENT_PROVIDES(CastorMerkleFlow)
