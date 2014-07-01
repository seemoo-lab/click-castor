#ifndef CLICK_CASTORFLOWSTUB_HH
#define CLICK_CASTORFLOWSTUB_HH
#include <click/element.hh>
#include <click/ipaddress.hh>
#include <click/hashtable.hh>
#include "castor.hh"
CLICK_DECLS

class CastorFlow;

typedef IPAddress 	Host;

typedef struct{
	String 			name;
	CastorFlow*		handle;
} FlowType;

typedef struct{
	FlowId		flow_id	;
	PacketId 	packet_id;
	uint16_t	packet_number;
	FlowAuth 	flow_auth[CASTOR_FLOWSIZE];
	ACKAuth 	ack_auth;
} PacketLabel;

typedef HashTable<Host, HashTable<Host, FlowType> > FlowMap;

class CastorFlowStub : public Element { 
	public:
		CastorFlowStub();
		~CastorFlowStub();
		
		const char *class_name() const	{ return "CastorFlowStub"; }
		const char *port_count() const	{ return PORTS_0_0; }
		const char *processing() const	{ return AGNOSTIC; }
		
		PacketLabel getPacketLabel(Host,Host);
		void registerFlowType(String name, CastorFlow* handle);

		void setDefaultType(uint8_t type);		

	private:
		FlowMap 			_flows;
		Vector<FlowType> 	_flowtypes;
		uint8_t 			_defaultType;

		FlowType * FlowTypeLookup(Host, Host);
};

CLICK_ENDDECLS
#endif

