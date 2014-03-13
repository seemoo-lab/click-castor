#include <click/config.h>
#include <click/confparse.hh>
#include "castor_flow_stub.hh"
#include "castor_flow.hh"

CLICK_DECLS

CastorFlowStub::CastorFlowStub(){
	_defaultType = 0;
}

CastorFlowStub::~ CastorFlowStub()
{}

PacketLabel CastorFlowStub::getPacketLabel(Host source, Host destination){
	//Check if module is initialized
	//TODO

	//Validate Inputs
	if(!source || ! destination)
		click_chatter("Invalid Parameters to get PacketLabel");

	//Determine the module to use
	FlowType* t = FlowTypeLookup(source, destination);
	//click_chatter("Using Protocol %s for Flow Generation", (t->name).c_str());

	//Forward request to matching packetflow
	PacketLabel lbl = t->handle->getPacketLabel(source, destination);

	//Check if Flow is still available
	//TODO

	return lbl;
}

void CastorFlowStub::registerFlowType(String name, CastorFlow * handle){
	FlowType type;
	type.name = String(name);
	type.handle = handle;

	_flowtypes.push_back(type);

	click_chatter("Flow registered at FlowStub" );
}

void CastorFlowStub::setDefaultType(uint8_t type){
	_defaultType = type;
}

FlowType * CastorFlowStub::FlowTypeLookup(Host source, Host destination){
	//Check if flow exist in database
	FlowType* ft;
	HashTable<Host, FlowType> * st = _flows.get_pointer(source);
	if(st){
		ft = st->get_pointer(destination);
		if(ft)
			return ft;
	}
	ft= &_flowtypes[_defaultType];
	return ft;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlowStub)

