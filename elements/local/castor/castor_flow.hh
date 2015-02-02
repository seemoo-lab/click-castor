#ifndef CLICK_CASTOR_FLOW_HH
#define CLICK_CASTOR_FLOW_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"
#include "castor_flow_stub.hh"

CLICK_DECLS

class CastorFlow : public Element { 
	public:
		CastorFlow() : _stub(0), _crypto(0) {}
		
		const char *class_name() const { return "CastorFlow"; }
		const char *port_count() const { return PORTS_0_0; }
		const char *processing() const { return AGNOSTIC; }

		int configure(Vector<String>&, ErrorHandler*);
		
		PacketLabel getPacketLabel(NodeId,NodeId);

	private:
		virtual bool hasFlow(NodeId, NodeId);
		virtual void createFlow(NodeId, NodeId);
		virtual PacketLabel useFlow(NodeId,NodeId);
		virtual void updateFlow(NodeId, NodeId);

		void registeratstub();
		CastorFlowStub * _stub;

	protected:
		Crypto* _crypto;
};

CLICK_ENDDECLS

#endif
