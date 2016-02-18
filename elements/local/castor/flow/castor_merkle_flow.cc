#include <click/config.h>
#include "castor_merkle_flow.hh"

CLICK_DECLS

CastorMerkleFlow::CastorMerkleFlow(size_t size, CastorFlowTable* flowtable, const Crypto* crypto) : size(size), height(MerkleTree::log2(size)), pos(0) {
	aauths = new Hash[size];
	pids = new Hash[size];
	crypto->random(n);
	// FIXME use actual end-to-end key
	Buffer<32> key;
	// Generate aauths from n
	crypto->stream(aauths->data(), size * sizeof(Hash), n.data(), key.data());
	for (int i = 0; i < size; i++) {
		crypto->hash(pids[i], aauths[i]);
	}
	tree = new MerkleTree(pids, size, *crypto);
	fid = tree->root();
	if (!flowtable->insert(tree))
		click_chatter("Could not add tree to flow table");
}

CastorMerkleFlow::~CastorMerkleFlow() {
	delete [] aauths;
	delete [] pids;
}

PacketLabel CastorMerkleFlow::freshLabel() {
	if (!isAlive())
		return PacketLabel();
	PacketLabel label;
	label.num = pos;
	label.size = height;
	label.n = n;
	label.fid = fid;
	label.pid = pids[pos];

	/* TODO legacy to not break Xcastor */
	label.fauth = new Hash[height];
	// Set flow authenticator
	tree->path_to_root(pos, label.fauth);
	label.aauth = aauths[pos];

	pos++;

	return label;
}

bool CastorMerkleFlow::isAlive() const {
	return pos < size;
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(MerkleTree)
ELEMENT_PROVIDES(CastorMerkleFlow)
