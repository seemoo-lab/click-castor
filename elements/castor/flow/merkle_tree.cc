/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <click/config.h>
#include "merkle_tree.hh"

CLICK_DECLS

MerkleTree::MerkleTree(const Hash in[], size_t length, const Crypto& crypto) : crypto(crypto) {
	if (!(length && !(length & (length - 1)))) {
		click_chatter("Input vector size must be a power of 2, but was %d", length);
		return;
	}
	_leaves = length;
	_height = log2(_leaves);
	_flat = new Hash[index(_height + 1)]; // +1 for root node

	unsigned int d = _height;
	// hash input values
	for (unsigned int k = 0; k < _leaves; k++) {
		crypto.hash(element(d, k), in[k]);
	}
	// create intermediate nodes up to the root
	for (; d > 0; d--) {
		for (unsigned int i = 0; i < nodes_at_depth(d); i += 2) {
			crypto.hash(element(d - 1, i >> 1),
					    element(d, i) + element(d, i + 1));
		}
	}
}

MerkleTree::MerkleTree(const Hash& root, size_t length, const Crypto& crypto) : crypto(crypto) {
	if (!(length && !(length & (length - 1)))) {
		click_chatter("Input vector size must be a power of 2, but was %d", length);
		return;
	}
	_leaves = length;
	_height = log2(_leaves);
	_flat = new Hash[index(_height + 1)](); // +1 -> for root node; () -> set nodes to zero
	_flat[0] = root;
}

MerkleTree::~MerkleTree() {
	delete [] _flat;
}

void MerkleTree::path_to_root(size_t k, Hash siblings[], height_t max) const {
	for (flat_rep_t i = index(_height, k), si = 0; i > 0 && si < max; i = parent(i), si++) {
		siblings[si] = _flat[sibling(i)];
	}
}

int MerkleTree::valid_leaf(size_t k, const Hash& in, const Hash siblings[], height_t n) const {
	// First hash the input
	Hash current;
	crypto.hash(current, in);
	flat_rep_t i = index(_height, k);
	for (height_t l = 0; i > 0 && l < n; i = parent(i), l++) {
		if (k & (1 << l)) { // sibling is left child
			crypto.hash(current, siblings[l] + current);
		} else { // sibling is right child
			crypto.hash(current, current + siblings[l]);
		}
	}
	if (_flat[i] == Hash())
		return -2;
	return (_flat[i] == current) ? 0 : -1;
}

void MerkleTree::add(size_t k, const Hash& in, const Hash siblings[], height_t n) {
	Hash current;
	crypto.hash(current, in);
	flat_rep_t i = index(_height, k);
	_flat[i] = current;
	for (height_t h = 0; i > 0 && h < n; i = parent(i), h++) {
		_flat[sibling(i)] = siblings[h];
		if (_flat[parent(i)] == Hash()) {
			if (k & (1u << h)) { // sibling is left child
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

bool MerkleTree::validate(size_t k, const Hash& in, const Hash siblings[], height_t h, const Hash& root, const Crypto& crypto) {
	// First hash the input
	Hash current;
	crypto.hash(current, in);
	for(height_t s = 0; s < h; s++) {
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
