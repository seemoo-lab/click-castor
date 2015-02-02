#ifndef CLICK_CASTOR_FLOW_MERKLE_HH
#define CLICK_CASTOR_FLOW_MERKLE_HH

#include <click/element.hh>
#include <click/vector.hh>
#include "castor_flow.hh"

CLICK_DECLS

class CastorFlowMerkle : public CastorFlow {
	public:
		const char *class_name() const	{ return "CastorFlowMerkle"; }

	private:
		typedef struct{
			unsigned int position;
			PacketLabel labels[CASTOR_FLOWSIZE];
		} Flow;

		typedef HashTable<NodeId, HashTable<NodeId, Flow> > Flows;
		Flows _flows;

		bool hasFlow(NodeId, NodeId);
		void createFlow(NodeId, NodeId);
		PacketLabel useFlow(NodeId,NodeId);
		void updateFlow(NodeId, NodeId);
};

CLICK_ENDDECLS

#endif
