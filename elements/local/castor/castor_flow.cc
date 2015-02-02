#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "castor_flow.hh"

CLICK_DECLS

int CastorFlow::configure(Vector<String> &conf, ErrorHandler *errh) {
	int r =  cp_va_kparse(conf, this, errh,
			"CastorFlowStub", cpkP+cpkM, cpElementCast, "CastorFlowStub", &_stub,
			"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &_crypto,
			cpEnd);
	registeratstub();
	return r;
}

/**
 * Register this Flow at the stub element
 */
void CastorFlow::registeratstub(){
	_stub->registerFlowType(class_name(), (CastorFlow*) this);
}

PacketLabel CastorFlow::getPacketLabel(NodeId source, NodeId destination){

	//Check if we already have a flow
	if(!hasFlow(source,destination)){
		//Create a new Flow
		createFlow(source, destination);
	}

	// Extract a new Label from the flow
	PacketLabel lbl = useFlow(source, destination);

	//Inform the Flow the we used it
	//Maybe the flow needs to be incremented
	updateFlow(source,destination);

	return lbl;
}

bool CastorFlow::hasFlow(NodeId, NodeId) {
	click_chatter("This message should not appear");
	return false;
}

void CastorFlow::createFlow(NodeId, NodeId) {
	click_chatter("This message should not appear");
}

PacketLabel CastorFlow::useFlow(NodeId, NodeId) {
	click_chatter("This message should not appear");
	return PacketLabel();
}

void CastorFlow::updateFlow(NodeId, NodeId) {
	click_chatter("This message should not appear");
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlow)
