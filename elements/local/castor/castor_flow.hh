#ifndef CLICK_CASTORFLOW_HH
#define CLICK_CASTORFLOW_HH
#include <click/element.hh>
#include "castor.hh"
#include "castor_flow_stub.hh"
#include "crypto.hh"
CLICK_DECLS

class CastorFlow : public Element { 
	public:
		CastorFlow();
		~CastorFlow();
		
		const char *class_name() const	{ return "CastorFlow"; }
		const char *port_count() const	{ return PORTS_0_0; }
		const char *processing() const	{ return AGNOSTIC; }

		int configure(Vector<String>&, ErrorHandler*);
		
		PacketLabel getPacketLabel(Host,Host);		

	private:
		virtual bool hasFlow(Host, Host);
		virtual void createFlow(Host, Host);
		virtual PacketLabel useFlow(Host,Host);
		virtual void updateFlow(Host, Host);

		void registeratstub();
		CastorFlowStub * _stub;

	protected:
		Crypto* _crypto;
};

CLICK_ENDDECLS
#endif

