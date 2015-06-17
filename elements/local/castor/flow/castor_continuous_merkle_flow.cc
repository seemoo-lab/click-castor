#include <click/config.h>
#include "castor_continuous_merkle_flow.hh"

CLICK_DECLS

CastorContinuousMerkleFlow::CastorContinuousMerkleFlow(NodeId src, NodeId dst, const Crypto* crypto) : CastorFlow(src, dst, crypto) {
	current = new CastorMerkleFlow(src, dst, crypto);
	next = new CastorMerkleFlow(src, dst, crypto);
}

CastorContinuousMerkleFlow::~CastorContinuousMerkleFlow() {
	delete current;
	delete next;
}

PacketLabel CastorContinuousMerkleFlow::freshLabel() {
	if (!current->isAlive()) {
		delete current;
		current = next;
		next = new CastorMerkleFlow(src, dst, crypto);
	}
	assert(current->isAlive());
	PacketLabel label = current->freshLabel();
	label.nfauth = calculateNextFlowAuth(next->tree->getRoot());
	return label;
}

bool CastorContinuousMerkleFlow::isAlive() const {
	return true;
}

inline NextFlowAuth CastorContinuousMerkleFlow::calculateNextFlowAuth(const SValue& root) const {
	return crypto->hashConvert(root);
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(TREE)
ELEMENT_PROVIDES(CastorContinuousMerkleFlow)
