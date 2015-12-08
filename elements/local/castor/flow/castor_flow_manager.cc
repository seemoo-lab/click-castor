#include <click/config.h>
#include <click/args.hh>
#include "castor_flow_manager.hh"
#include "castor_merkle_flow.hh"

CLICK_DECLS

CastorFlowManager::CastorFlowManager() : _crypto(NULL),
_flows(HashTable<NodeId, HashTable<NodeId, CastorFlow*> >(HashTable<NodeId, CastorFlow*>(NULL)))
{
}

int CastorFlowManager::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Crypto", ElementCastArg("Crypto"), _crypto)
			.complete();
}

PacketLabel CastorFlowManager::getPacketLabel(NodeId src, NodeId dst) {
	if (src.empty() || dst.empty() || src.is_multicast()) {
		click_chatter("Invalid source or destination address");
		return PacketLabel();
	}

	CastorFlow* flow = createFlowIfNotExists(src, dst);

	return flow->freshLabel();
}

CastorFlow* CastorFlowManager::createFlowIfNotExists(NodeId src, NodeId dst) {
	CastorFlow*& flow = _flows[src][dst];

	if (!flow) {
		flow = createNewFlow();
	} else if (!flow->isAlive()) {
		delete flow;
		flow = createNewFlow();
	} else {
	}
	assert(flow && flow->isAlive());

	return flow;
}

CastorFlow* CastorFlowManager::createNewFlow() {
	return new CastorMerkleFlow(_crypto);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlowManager)
