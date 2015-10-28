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
	CastorFlow* flow;

	HashTable<NodeId, CastorFlow*> * t = _flows.get_pointer(src);
	if (!t) {
		_flows.set(src, HashTable<NodeId, CastorFlow*>());
		t = _flows.get_pointer(src);
	}
	CastorFlow** f = t->get_pointer(dst);
	if (!f) {
		flow = createNewFlow(src, dst);
		t->set(dst, flow);
	} else if (!(*f)->isAlive()) {
		delete *f;
		flow = createNewFlow(src, dst);
		t->set(dst, flow);
	} else {
		flow = *f;
	}
	assert(flow != NULL && flow->isAlive());

	return flow;
}

CastorFlow* CastorFlowManager::createNewFlow(NodeId src, NodeId dst) {
	return new CastorMerkleFlow(src, dst, _crypto);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlowManager)
