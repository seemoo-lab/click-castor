#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "castor_flow.hh"

CLICK_DECLS

CastorFlow::CastorFlow(){
}

CastorFlow::~ CastorFlow()
{}

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

PacketLabel CastorFlow::getPacketLabel(Host source, Host destination){

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

bool CastorFlow::hasFlow(Host, Host) {
	click_chatter("This message should not appear");
	return false;
}

void CastorFlow::createFlow(Host, Host) {
	click_chatter("This message should not appear");
}

PacketLabel CastorFlow::useFlow(Host, Host) {
	click_chatter("This message should not appear");
	return PacketLabel();
}

void CastorFlow::updateFlow(Host, Host) {
	click_chatter("This message should not appear");
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlow)
