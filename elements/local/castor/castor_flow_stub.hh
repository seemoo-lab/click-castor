#ifndef CLICK_CASTOR_FLOW_STUB_HH
#define CLICK_CASTOR_FLOW_STUB_HH
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
	uint8_t 	flow_id		[CASTOR_HASHLENGTH];
	uint8_t 	packet_id	[CASTOR_HASHLENGTH];
	FlowAuth 	flow_auth 	[CASTOR_FLOWSIZE];
	uint8_t 	enc_ack_auth[CASTOR_HASHLENGTH];
} PacketLabel;

typedef HashTable<Host, HashTable<Host, FlowType> > FlowMap;

class CastorFlowStub : public Element { 
	public:
		CastorFlowStub();
		~CastorFlowStub();
		
		const char *class_name() const	{ return "CastorFlowStub"; }
		const char *port_count() const	{ return PORTS_0_0; }
		const char *processing() const	{ return AGNOSTIC; }

		//int configure(Vector<String>&, ErrorHandler*);
		
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

