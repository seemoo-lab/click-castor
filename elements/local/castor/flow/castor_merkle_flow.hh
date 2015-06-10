#ifndef CLICK_CASTOR_MERKLE_FLOW_HH
#define CLICK_CASTOR_MERKLE_FLOW_HH

#include <click/vector.hh>
#include "castor_flow.hh"
#include "../tree.hh"

CLICK_DECLS

class CastorMerkleFlow : public CastorFlow {
public:
	CastorMerkleFlow(NodeId src, NodeId dst, const Crypto* crypto);
	~CastorMerkleFlow();

	PacketLabel freshLabel();
	bool isAlive() const;

private:
	const MerkleTree* tree;
	Vector<SValue> aauths;
	Vector<SValue> pids;
	size_t pos;
};

CLICK_ENDDECLS

#endif
