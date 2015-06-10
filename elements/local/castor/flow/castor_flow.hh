#ifndef CLICK_CASTOR_FLOW_HH
#define CLICK_CASTOR_FLOW_HH

#include <click/element.hh>
#include "../castor.hh"
#include "../crypto/crypto.hh"

CLICK_DECLS

class PacketLabel {
public:
	size_t num;
	FlowId fid;
	FlowAuth fauth;
	PacketId pid;
	AckAuth aauth;
#ifdef CASTOR_CONTINUOUS_FLOW
	NextFlowAuth nfauth;
#endif
};

class CastorFlow {
public:
	CastorFlow(NodeId src, NodeId dst, const Crypto* crypto) : src(src), dst(dst), crypto(crypto) {}
	virtual ~CastorFlow() {}

	/**
	 * Returns a fresh label or a dummy label if this flow is dead.
	 */
	virtual PacketLabel freshLabel() = 0;
	/**
	 * Returns true if this flow is still alive, i.e., if there exist more fresh labels.
	 * Returns false otherwise.
	 */
	virtual bool isAlive() const = 0;

protected:
	const NodeId src;
	const NodeId dst;
	const Crypto* crypto;
};

CLICK_ENDDECLS

#endif
