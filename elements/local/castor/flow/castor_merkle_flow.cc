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
	label.fid = tree->getRoot();
	label.pid = pids[pos];

	// Set flow authenticator
	Vector<Hash> siblings = tree->getSiblings(pos);
	assert(siblings.size() == CASTOR_FLOWAUTH_ELEM);
	for (int j = 0; j < siblings.size(); j++)
		label.fauth[j] = siblings[j];

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
