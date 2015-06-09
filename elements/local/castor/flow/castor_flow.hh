#ifndef CLICK_CASTOR_FLOW_HH
#define CLICK_CASTOR_FLOW_HH

#include <click/element.hh>
#include "../castor.hh"
#include "../crypto/crypto.hh"

CLICK_DECLS

class PacketLabel {
public:
//	PacketLabel(size_t num, const FlowId& fid, const FlowAuth& fauth, const PacketId& pid, const AckAuth& aauth)
//		: num(num), fid(fid), fauth(fauth), pid(pid), aauth(aauth) {}
	size_t num;
	FlowId fid;
	FlowAuth fauth;
	PacketId pid;
	AckAuth aauth;
};

class CastorFlow {
public:
	CastorFlow(NodeId src, NodeId dst, const Crypto* crypto) {
		(void) src; (void) dst; (void) crypto;
	}
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
};

CLICK_ENDDECLS

#endif
