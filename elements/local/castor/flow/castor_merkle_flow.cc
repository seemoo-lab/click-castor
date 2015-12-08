#include <click/config.h>
#include "castor_merkle_flow.hh"

CLICK_DECLS

CastorMerkleFlow::CastorMerkleFlow(NodeId src, NodeId dst, const Crypto* crypto) : CastorFlow(src, dst, crypto), pos(0) {
	Vector<SValue> tmp;
	for (int i = 0; i < CASTOR_FLOWSIZE; i++) {
		crypto->random(aauths[i]);
		crypto->hash(pids[i], aauths[i]);
		tmp.push_back(crypto->convert(pids[i]));
	}
	tree = new MerkleTree(tmp, *crypto);
}

CastorMerkleFlow::~CastorMerkleFlow() {
	delete tree;
}

PacketLabel CastorMerkleFlow::freshLabel() {
	if (!isAlive())
		return PacketLabel();

	PacketLabel label;
	label.num = pos;
	label.fid = crypto->convert(tree->getRoot());
	label.pid = pids[pos];

	// Set flow authenticator
	Vector<SValue> siblings = tree->getSiblings(pos);
	assert(siblings.size() == CASTOR_FLOWAUTH_ELEM);
	for (int j = 0; j < siblings.size(); j++)
		label.fauth[j] = crypto->convert(siblings[j]);

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
