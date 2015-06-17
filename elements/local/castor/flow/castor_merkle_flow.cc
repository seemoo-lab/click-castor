#include <click/config.h>
#include "castor_merkle_flow.hh"

#define CASTOR_FLOWSIZE (1<<CASTOR_FLOWAUTH_ELEM) // Number of elements in a merkle hash tree

CLICK_DECLS

CastorMerkleFlow::CastorMerkleFlow(NodeId src, NodeId dst, const Crypto* crypto) : CastorFlow(src, dst, crypto), pos(0) {
	aauths.reserve(CASTOR_FLOWSIZE);
	pids.reserve(CASTOR_FLOWSIZE);

	for (int i = 0; i < CASTOR_FLOWSIZE; i++) {
		aauths.push_back(crypto->random(sizeof(AckAuth)));
		pids.push_back(crypto->hash(aauths[i]));
	}

	tree = new MerkleTree(pids, *crypto);
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
	label.pid = crypto->convert(pids[pos]);

	// Set flow authenticator
	Vector<SValue> siblings = tree->getSiblings(pos);
	assert(siblings.size() == CASTOR_FLOWAUTH_ELEM);
	for (int j = 0; j < siblings.size(); j++)
		label.fauth[j] = crypto->convert(siblings[j]);

	label.aauth = crypto->convert(aauths[pos]);

	pos++;

	return label;
}

bool CastorMerkleFlow::isAlive() const {
	assert(aauths.size() >= 0);
	return pos < (size_t) aauths.size();
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(TREE)
ELEMENT_PROVIDES(CastorMerkleFlow)
