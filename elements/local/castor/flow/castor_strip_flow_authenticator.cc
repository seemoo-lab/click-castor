#include <click/config.h>
#include "castor_strip_flow_authenticator.hh"
#include "../castor.hh"

CLICK_DECLS

Packet* CastorStripFlowAuthenticator::simple_action(Packet *p) {
	CastorPkt pkt = *reinterpret_cast<const CastorPkt*>(p->data());
	Nonce n = pkt.syn() ? *reinterpret_cast<const CastorPkt*>(p->data())->n() : Nonce();

	if (pkt.fasize() == 0)
		return p;

	WritablePacket* q = p->uniqueify();

	// remove flow authenticator
	unsigned int diff = pkt.fasize() * pkt.hsize;
	q->pull(diff);

	// update length fields
	pkt.len = htons(ntohs(pkt.len) - diff);
	pkt.set_fasize(0);

	// write back header to Packet
	memcpy(q->data(), &pkt, sizeof(pkt));
	if (pkt.syn())
		*reinterpret_cast<CastorPkt*>(q->data())->n() = n;

	return q;
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorStripFlowAuthenticator)
