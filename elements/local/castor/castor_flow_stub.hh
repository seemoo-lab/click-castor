#ifndef CLICK_CASTOR_FLOW_STUB_HH
#define CLICK_CASTOR_FLOW_STUB_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "castor.hh"

CLICK_DECLS

class CastorFlow;

typedef struct {
	String name;
	CastorFlow* handle;
} FlowType;

typedef struct {
	FlowId flow_id;
	PacketId packet_id;
	uint16_t packet_number;
	FlowAuth flow_auth;
	AckAuth ack_auth;
} PacketLabel;

typedef HashTable<NodeId, HashTable<NodeId, FlowType> > FlowMap;

class CastorFlowStub: public Element {
public:
	CastorFlowStub() : _defaultType(0) {}
		
	const char *class_name() const	{ return "CastorFlowStub"; }
	const char *port_count() const	{ return PORTS_0_0; }
	const char *processing() const	{ return AGNOSTIC; }

	PacketLabel getPacketLabel(NodeId, NodeId);
	void registerFlowType(String name, CastorFlow* handle);

	void setDefaultType(uint8_t type);

private:
	FlowMap _flows;
	Vector<FlowType> _flowtypes;
	uint8_t _defaultType;

	FlowType* FlowTypeLookup(NodeId, NodeId);
};

CLICK_ENDDECLS

#endif
