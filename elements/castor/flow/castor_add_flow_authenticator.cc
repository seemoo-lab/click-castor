#include <click/config.h>
#include <click/args.hh>
#include "castor_add_flow_authenticator.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorAddFlowAuthenticator::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Flows", ElementCastArg("CastorFlowTable"), flowtable)
			.read_or_set_p("FORCE_FULL", force_full_auth, false)
			.complete();
}

Packet* CastorAddFlowAuthenticator::simple_action(Packet *p) {
	const CastorPkt& pkt = *reinterpret_cast<const CastorPkt*>(p->data());

	// Send full-sized flow authenticator if we broadcast
	bool full_auth = force_full_auth ||
					 CastorAnno::dst_id_anno(p) == NeighborId::make_broadcast();

	uint8_t new_fasize = full_auth ? pkt.fsize() : fauth_size(ntohs(pkt.kpkt), pkt.fsize(), pkt.fid, CastorAnno::dst_id_anno(p));
	uint8_t old_fasize = pkt.fasize();

	if (new_fasize == old_fasize) {
		return p;
	}

	// Save header
	CastorPkt header = pkt;
	header.set_fasize(new_fasize);
	header.len = htons(ntohs(header.len) + (unsigned int) (new_fasize - old_fasize) * header.hsize);
	Nonce n = pkt.syn() ? *pkt.n() : Nonce();

	// Resize Packet
	WritablePacket* q = p->uniqueify();
	if (new_fasize > old_fasize) {
		q = q->push((new_fasize - old_fasize) * header.hsize);
	} else {
		q->pull((old_fasize - new_fasize) * header.hsize);
	}

	memcpy(q->data(), &header, sizeof(CastorPkt));
	CastorPkt& newPkt = *reinterpret_cast<CastorPkt*>(q->data());
	if (header.syn())
		*newPkt.n() = n;
	flowtable->get(header.fid).tree()->path_to_root(ntohs(header.kpkt), newPkt.fauth(), header.fasize());

	return q;
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorAddFlowAuthenticator)
