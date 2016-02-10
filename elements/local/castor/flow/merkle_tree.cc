#include <click/config.h>
#include "merkle_tree.hh"

CLICK_DECLS

MerkleTree::MerkleTree(const Hash in[], unsigned int length, const Crypto& crypto) {
	if (!(length && !(length & (length - 1)))) {
		click_chatter("Input vector size must be a power of 2, but was %d", length);
		return;
	}
	leaves = length;
	height = log2(leaves) + 1;
	_flat = new Hash[index(height)];

	unsigned int l = height - 1;
	// hash input values
	for (int i = 0; i < leaves; i++) {
		crypto.hash(element(l, i), in[i]);
	}
	// create intermediate nodes up to the root
	for (; l > 0; l--) {
		for (int i = 0; i < nodes_per_level(l); i += 2) {
			crypto.hash(element(l - 1, i >> 1),
					    element(l, i) + element(l, i + 1));
		}
	}
}

MerkleTree::~MerkleTree() {
	delete [] _flat;
}

const Hash& MerkleTree::root() const {
	return _flat[0];
}

void MerkleTree::path_to_root(unsigned int leaf, Hash siblings[]) const {
	for (unsigned int i = index(height - 1, leaf), si = 0; i > 0; i = parent(i), si++) {
		siblings[si] = _flat[sibling(i)];
	}
}

bool MerkleTree::validate(unsigned int i, const Hash& in, const Hash siblings[], unsigned int h, const Hash& root, const Crypto& crypto) {
	// First hash the input
	Hash current;
	crypto.hash(current, in);
	for(int s = 0; s < h; s++) {
		if (i & (1 << s)) { // sibling is left child
			crypto.hash(current, siblings[s] + current);
		} else { // sibling is right child
			crypto.hash(current, current + siblings[s]);
		}
	}
	return current == root;
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(MerkleTree)
