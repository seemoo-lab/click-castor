#include <click/config.h>
#include "merkle_tree.hh"

CLICK_DECLS

MerkleTree::MerkleTree(const Hash in[], unsigned int length, const Crypto& crypto) : crypto(crypto) {
	if (!(length && !(length & (length - 1)))) {
		click_chatter("Input vector size must be a power of 2, but was %d", length);
		return;
	}
	leaves = length;
	height = log2(leaves) + 1;
	_flat = new Hash[index(height)];

	unsigned int l = height - 1;
	// hash input values
	for (int k = 0; k < leaves; k++) {
		crypto.hash(element(l, k), in[k]);
	}
	// create intermediate nodes up to the root
	for (; l > 0; l--) {
		for (int i = 0; i < nodes_per_level(l); i += 2) {
			crypto.hash(element(l - 1, i >> 1),
					    element(l, i) + element(l, i + 1));
		}
	}
}

MerkleTree::MerkleTree(const Hash& root, unsigned int length, const Crypto& crypto) : crypto(crypto) {
	if (!(length && !(length & (length - 1)))) {
		click_chatter("Input vector size must be a power of 2, but was %d", length);
		return;
	}
	leaves = length;
	height = log2(leaves) + 1;
	_flat = new Hash[index(height)] { root }; // set first element to 'root' rest to zero
}

MerkleTree::~MerkleTree() {
	delete [] _flat;
}

const Hash& MerkleTree::root() const {
	return _flat[0];
}

void MerkleTree::path_to_root(unsigned int k, Hash siblings[], unsigned int max) const {
	for (unsigned int i = index(height - 1, k), si = 0; i > 0 && si < max; i = parent(i), si++) {
		siblings[si] = _flat[sibling(i)];
	}
}

bool MerkleTree::valid_leaf(unsigned int k, const Hash& in, const Hash siblings[], unsigned int n) const {
	// First hash the input
	Hash current;
	crypto.hash(current, in);
	unsigned int i = index(height - 1, k);
	for (unsigned int l = 0; i > 0 && l < n; i = parent(i), l++) {
		if (k & (1 << l)) { // sibling is left child
			crypto.hash(current, siblings[l] + current);
		} else { // sibling is right child
			crypto.hash(current, current + siblings[l]);
		}
	}
	return _flat[i] == current;
}

void MerkleTree::add(unsigned int k, const Hash& in, const Hash siblings[], unsigned int n) {
	Hash current;
	crypto.hash(current, in);
	for (unsigned int i = index(height - 1, k), l = 0; i > 0 && l < n; i = parent(i), l++) {
		_flat[i] = current;
		_flat[sibling(i)] = siblings[l];
		if (_flat[parent(i)] == Hash()) {
			if (k & (1 << l)) { // sibling is left child
				crypto.hash(_flat[parent(i)], _flat[sibling(i)] + _flat[i]);
			} else { // sibling is right child
				crypto.hash(_flat[parent(i)], _flat[i] + _flat[sibling(i)]);
			}
		} else {
			// we have valid tree elements from here on up
			break;
		}
	}
}

bool MerkleTree::validate(unsigned int k, const Hash& in, const Hash siblings[], unsigned int h, const Hash& root, const Crypto& crypto) {
	// First hash the input
	Hash current;
	crypto.hash(current, in);
	for(int s = 0; s < h; s++) {
		if (k & (1 << s)) { // sibling is left child
			crypto.hash(current, siblings[s] + current);
		} else { // sibling is right child
			crypto.hash(current, current + siblings[s]);
		}
	}
	return current == root;
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(MerkleTree)
