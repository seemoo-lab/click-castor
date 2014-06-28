#ifndef CLICK_CASTORFLOWMERKLE_HH
#define CLICK_CASTORFLOWMERKLE_HH

#include <click/element.hh>
#include "castor_flow.hh"
#include <click/vector.hh>

#define CASTOR_REAL_FLOWSIZE (1 << (CASTOR_FLOWSIZE + 1))

CLICK_DECLS

typedef uint8_t FlowID[CASTOR_HASHLENGTH]; 

typedef struct{
	uint8_t position;
	PacketLabel labels[CASTOR_REAL_FLOWSIZE];
} Flow;


typedef HashTable<Host, HashTable<Host, Flow> > Flows;

class CastorFlowMerkle : public CastorFlow {

	public:
		const char *class_name() const	{ return "CastorFlowMerkle"; }

	private:
		bool hasFlow(Host, Host);
		void createFlow(Host, Host);
		PacketLabel useFlow(Host,Host);
		void updateFlow(Host, Host);

		Flows _flows;
};

CLICK_ENDDECLS

#endif
