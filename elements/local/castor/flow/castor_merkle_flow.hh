#ifndef CLICK_CASTOR_MERKLE_FLOW_HH
#define CLICK_CASTOR_MERKLE_FLOW_HH

#include <click/vector.hh>
#include "../crypto/crypto.hh"
#include "merkle_tree.hh"
#include "castor_flow.hh"

CLICK_DECLS

class CastorMerkleFlow : public CastorFlow {
public:
	CastorMerkleFlow(size_t size, const Crypto* crypto);
	~CastorMerkleFlow();

	PacketLabel freshLabel();
	bool isAlive() const;
private:
	const MerkleTree* tree;
	Hash* aauths;
	Hash* pids;
	const unsigned int size;
	const unsigned int height;
	unsigned int pos;
};

CLICK_ENDDECLS

#endif
