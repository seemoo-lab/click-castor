#ifndef CLICK_CASTOR_FLOW_MERKLE_HH
#define CLICK_CASTOR_FLOW_MERKLE_HH
#include <click/element.hh>
#include "castor_flow.hh"
#include <click/vector.hh>
#include "castor.hh"
CLICK_DECLS

typedef uint8_t FlowID[CASTOR_HASHLENGTH]; 

typedef struct{
	uint8_t position;
	PacketLabel labels[CASTOR_FLOWSIZE];
} Flow;



typedef HashTable<Host, HashTable<Host, Flow> > Flows;


class CastorFlowMerkle : public CastorFlow {

	private:
		bool hasFlow(Host, Host);
		void createFlow(Host, Host);
		PacketLabel useFlow(Host,Host);
		void updateFlow(Host, Host);

		//const String _name = "MerkleTree Flow";
		Flows _flows;
};

CLICK_ENDDECLS
#endif

