#include <click/config.h>
#include "castor_merkle_flow.hh"

CLICK_DECLS

CastorMerkleFlow::CastorMerkleFlow(const Crypto* crypto) : pos(0) {
	for (int i = 0; i < CASTOR_FLOWSIZE; i++) {
		crypto->random(aauths[i]);
		crypto->hash(pids[i], aauths[i]);
	}
	tree = new MerkleTree(pids, CASTOR_FLOWSIZE, *crypto);
}

CastorMerkleFlow::~CastorMerkleFlow() {
	delete tree;
}

PacketLabel CastorMerkleFlow::freshLabel() {
	if (!isAlive())
		return PacketLabel();
	PacketLabel label;
	label.num = pos;
	label.fid = tree->root();
	label.pid = pids[pos];

	// Set flow authenticator
	tree->path_to_root(pos, label.fauth.elem);
	label.aauth = aauths[pos];

	pos++;

	return label;
}

bool CastorMerkleFlow::isAlive() const {
	return pos < (size_t) CASTOR_FLOWSIZE;
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
ELEMENT_PROVIDES(CastorMerkleFlow)
