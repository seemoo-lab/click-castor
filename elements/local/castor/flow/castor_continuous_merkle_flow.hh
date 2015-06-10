#ifndef CLICK_CASTOR_CONTINUOUS_MERKLE_FLOW_HH
#define CLICK_CASTOR_CONTINUOUS_MERKLE_FLOW_HH

#include "castor_flow.hh"
#include "castor_merkle_flow.hh"

#ifdef CASTOR_CONTINUOUS_FLOW

CLICK_DECLS

class CastorContinuousMerkleFlow : public CastorFlow {
public:
	CastorContinuousMerkleFlow(NodeId src, NodeId dst, const Crypto* crypto);
	~CastorContinuousMerkleFlow();

	PacketLabel freshLabel();
	bool isAlive() const;

private:
	CastorMerkleFlow* current;
	CastorMerkleFlow* next;

	NextFlowAuth calculateNextFlowAuth(const SValue& root) const;
};

CLICK_ENDDECLS

#endif

#endif
