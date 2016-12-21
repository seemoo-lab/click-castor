#ifndef CLICK_CASTOR_MERKLE_FLOW_HH
#define CLICK_CASTOR_MERKLE_FLOW_HH

#include <click/vector.hh>
#include "../crypto/crypto.hh"
#include "merkle_tree.hh"
#include "castor_flow_table.hh"

CLICK_DECLS

class PacketLabel {
public:
	unsigned int num;
	unsigned int size;
	FlowId fid;
	PacketId pid;
	AckAuth aauth;
	Nonce n;
};

class CastorMerkleFlow {
public:
	CastorMerkleFlow(size_t size, const NodeId& dst, CastorFlowTable* flowtable, const Crypto* crypto);
	~CastorMerkleFlow();

	PacketLabel freshLabel();
	FlowId getFlowId();
	bool isAlive() const;
private:
	MerkleTree* tree;
	Nonce n;
	Hash  fid;
	Hash* aauths;
	Hash* pids;
	const unsigned int size;
	const unsigned int height;
	unsigned int pos;
};

CLICK_ENDDECLS

#endif
