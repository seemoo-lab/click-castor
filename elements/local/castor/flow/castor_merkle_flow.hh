#ifndef CLICK_CASTOR_MERKLE_FLOW_HH
#define CLICK_CASTOR_MERKLE_FLOW_HH

#include <click/vector.hh>
#include "../crypto/crypto.hh"
#include "merkle_tree.hh"
#include "castor_flow.hh"

#define CASTOR_FLOWSIZE (1<<CASTOR_FLOWAUTH_ELEM) // Number of elements in a merkle hash tree

CLICK_DECLS

class CastorMerkleFlow : public CastorFlow {
public:
	CastorMerkleFlow(const Crypto* crypto);
	~CastorMerkleFlow();

	PacketLabel freshLabel();
	bool isAlive() const;
private:
	const MerkleTree* tree;
	Hash aauths[CASTOR_FLOWSIZE];
	Hash pids[CASTOR_FLOWSIZE];
	size_t pos;
};

CLICK_ENDDECLS

#endif
