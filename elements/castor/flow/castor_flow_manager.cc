#include <click/config.h>
#include <click/args.hh>
#include "castor_flow_manager.hh"
#include "castor_merkle_flow.hh"

CLICK_DECLS

CastorFlowManager::CastorFlowManager() : _crypto(NULL),
_flows(HashTable<NodeId, HashTable<NodeId, CastorMerkleFlow*> >(HashTable<NodeId, CastorMerkleFlow*>(NULL)))
{
}

int CastorFlowManager::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("SIZE", _flowsize)
			.read_mp("FlowTable", ElementCastArg("CastorFlowTable"), _flowtable)
			.read_mp("Crypto", ElementCastArg("Crypto"), _crypto)
			.complete();
}

const FlowId &CastorFlowManager::getCurrentFlowId(NodeId src, NodeId dst) {
	CastorMerkleFlow* flow = createFlowIfNotExists(src, dst);

	return flow->getFlowId();
}

PacketLabel CastorFlowManager::getPacketLabel(NodeId src, NodeId dst) {
	assert(!src.empty() || !dst.empty() || !src.is_multicast());

	CastorMerkleFlow* flow = createFlowIfNotExists(src, dst);

	return flow->freshLabel();
}

CastorMerkleFlow* CastorFlowManager::createFlowIfNotExists(NodeId src, NodeId dst) {
	CastorMerkleFlow*& flow = _flows[src][dst];

	if (!flow) {
		flow = createNewFlow(src, dst);
	} else if (!flow->isAlive()) {
		delete flow;
		flow = createNewFlow(src, dst);
	} else {
	}
	assert(flow && flow->isAlive());

	return flow;
}

CastorMerkleFlow* CastorFlowManager::createNewFlow(const NodeId& src, const NodeId& dst) {
	(void) src;
	return new CastorMerkleFlow(_flowsize, dst, _flowtable, _crypto);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlowManager)
