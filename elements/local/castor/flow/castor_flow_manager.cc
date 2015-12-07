#include <click/config.h>
#include <click/confparse.hh>
#include "castor_flow_manager.hh"
#include "castor_merkle_flow.hh"

CLICK_DECLS

int CastorFlowManager::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh, "CRYPT", cpkP + cpkM, cpElementCast,
			"Crypto", &_crypto,
			cpEnd);
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
		flow = createNewFlow(src, dst);
	} else if (!flow->isAlive()) {
		delete flow;
		flow = createNewFlow(src, dst);
	}
	assert(flow && flow->isAlive());

	return flow;
}

CastorFlow* CastorFlowManager::createNewFlow(NodeId src, NodeId dst) {
	return new CastorMerkleFlow(src, dst, _crypto);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlowManager)
