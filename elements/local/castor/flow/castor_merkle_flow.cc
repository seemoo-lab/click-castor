#include <click/config.h>
#include "castor_merkle_flow.hh"

CLICK_DECLS

CastorMerkleFlow::CastorMerkleFlow(NodeId src, NodeId dst, const Crypto* crypto) : CastorFlow(src, dst, crypto), _crypto(crypto), _pos(0) {
	// 'src' and 'dst' are not used here but could be useful in the future or for other flow types
	(void) src; (void) dst;

	aauths.reserve(CASTOR_FLOWSIZE);
	pids.reserve(CASTOR_FLOWSIZE);

	for (int i = 0; i < CASTOR_FLOWSIZE; i++) {
		aauths.push_back(_crypto->random(sizeof(AckAuth)));
		pids.push_back(_crypto->hash(aauths[i]));
	}

	_tree = new MerkleTree(pids, *_crypto);
}

CastorMerkleFlow::~CastorMerkleFlow() {
	delete _tree;
}

PacketLabel CastorMerkleFlow::freshLabel() {
	if (!isAlive())
		return PacketLabel();

	PacketLabel label;
	label.num = _pos;
	label.fid = Hash(_tree->getRoot().begin());
	label.pid = Hash(pids[_pos].begin());

	// Set flow authenticator
	Vector<SValue> siblings = _tree->getSiblings(_pos);
	assert(siblings.size() == CASTOR_FLOWAUTH_ELEM);
	for (int j = 0; j < siblings.size(); j++)
		label.fauth[j] = _crypto->convert(siblings[j]);

	label.aauth = _crypto->convert(aauths[_pos]);

	_pos++;

	return label;
}

bool CastorMerkleFlow::isAlive() const {
	return _pos < sizeof(aauths);
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(TREE)
ELEMENT_PROVIDES(CastorMerkleFlow)
