#include <click/config.h>
#include <click/args.hh>
#include "castor_add_flow_authenticator.hh"
#include "castor.hh"
#include "castor_anno.hh"
#include "flow/merkle_tree.hh"

CLICK_DECLS

int CastorAddFlowAuthenticator::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("Flows", ElementCastArg("CastorFlowTable"), flowtable)
			.complete();
}

Packet* CastorAddFlowAuthenticator::simple_action(Packet *p) {
	const CastorPkt& pkt = *reinterpret_cast<const CastorPkt*>(p->data());

	// Send full-sized flow authenticator if we broadcast
	// FIXME or if a route change occured
	unsigned int new_fasize = (CastorAnno::dst_id_anno(p) == NeighborId::make_broadcast()) ?
							  pkt.fsize : fauth_size(ntohs(pkt.kpkt), pkt.fsize);
	unsigned int old_fasize = pkt.fasize;

	if (new_fasize == old_fasize) {
		return p;
	}

	// Save header
	CastorPkt header = pkt;
	header.len = ntohs(sizeof(CastorPkt) + header.hsize * new_fasize);
	header.fasize = new_fasize;

	// Resize Packet
	WritablePacket* q = p->uniqueify();
	if (new_fasize > old_fasize) {
		q = q->push((new_fasize - old_fasize) * header.hsize);
	} else {
		q->pull((old_fasize - new_fasize) * header.hsize);
	}

	memcpy(q->data(), &header, sizeof(CastorPkt));

	MerkleTree* tree = flowtable->get(header.fid, header.fsize);
	tree->path_to_root(ntohs(header.kpkt), reinterpret_cast<Hash*>(q->data() + sizeof(CastorPkt)), header.fasize);

	return q;
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
EXPORT_ELEMENT(CastorAddFlowAuthenticator)
