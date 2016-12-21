#ifndef CLICK_CASTOR_FLOW_HH
#define CLICK_CASTOR_FLOW_HH

#include <click/element.hh>
#include "../castor.hh"

CLICK_DECLS

class PacketLabel {
public:
	unsigned int num;
	unsigned int size;
	FlowId fid;
	PacketId pid;
	AckAuth aauth;
	Nonce n;
};

class CastorFlow {
public:
	CastorFlow() {}
	virtual ~CastorFlow() {}

	/**
	 * Returns a fresh label or a dummy label if this flow is dead.
	 */
	virtual PacketLabel freshLabel() = 0;
	/**
	 * Returns the current flow id.
	 */
	virtual FlowId getFlowId() = 0;
	/**
	 * Returns true if this flow is still alive, i.e., if there exist more fresh labels.
	 * Returns false otherwise.
	 */
	virtual bool isAlive() const = 0;
};

CLICK_ENDDECLS

#endif
