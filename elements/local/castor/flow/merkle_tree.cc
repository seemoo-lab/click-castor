#include <click/config.h>
#include "merkle_tree.hh"

CLICK_DECLS

class MerkleTree::Node {
public:
	Node(const Hash& data = Hash(), Node* parent = 0, Node* left_child = 0, Node* right_child = 0) :
			data(data), parent(parent), lc(left_child), rc(right_child) {
		if (lc) lc->parent = this;
		if (rc) rc->parent = this;
	}
	~Node() {
		delete lc;
		delete rc;
	}

	bool is_root() const { return parent == 0; }
	bool is_leaf() const { return lc == 0 && rc == 0; }
	bool is_left_child() const { return !is_root() && (parent->lc == this); }
	bool is_right_child() const { return !is_root() && (parent->rc == this); }
	Node* sibling() const {
		if(is_left_child())
			return parent->rc;
		else if(is_right_child())
			return parent->lc;
		else
			return 0;
	}

	Hash data;
	Node* parent;
	Node* lc;
	Node* rc;
};

MerkleTree::MerkleTree(const Hash in[], unsigned int length, const Crypto& crypto) {
	if (!(length && !(length & (length - 1)))) {
		click_chatter("Input vector size must be a power of 2, but was %d", length);
		return;
	}

	// Create the leaves
	_leaves = new Node*[length];
	for (int i = 0; i < length; i++) {
		_leaves[i] = new Node();
		crypto.hash(_leaves[i]->data, in[i]);
	}
	// Build the tree
	Node** layer = new Node*[length];
	memcpy(layer, _leaves, length * sizeof(Node*));
	for (unsigned int h = log2(length); h > 0; h--) {
		for (int i = 0; i < (1 << h); i += 2) {
			// layer[i/2] = hash(layer[i] || layer[i+1]) is equivalent to
			// parent = hash(lc || rc)
			Node* lc = layer[i];
			Node* rc = layer[i + 1];
			layer[i >> 1] = new Node(Hash(), 0, lc, rc);
			crypto.hash(layer[i >> 1]->data,
					    lc->data + rc->data);
		}
	}
	_root = layer[0];
	delete [] layer;
}

MerkleTree::~MerkleTree() {
	// Nodes delete children recursively
	delete _root;
	delete [] _leaves;
}

const Hash& MerkleTree::getRoot() const {
	return _root->data;
}

Vector<Hash> MerkleTree::getSiblings(int i) const {
	Vector<Hash> siblings;
	Node* node = _leaves[i];
	while(!node->is_root()) {
		siblings.push_back(node->sibling()->data);
		node = node->parent;
	}
	return siblings;
}

bool MerkleTree::isValidMerkleTree(unsigned int i, const Hash& in, const Vector<Hash>& siblings, const Hash& root, const Crypto& crypto) {
	// First hash the input
	Hash current;
	crypto.hash(current, in);
	for(int s = 0; s < siblings.size(); s++) {
		if(i & (1 << s)) { // sibling is left child
			crypto.hash(current, siblings[s] + current);
		} else { // sibling is right child
			crypto.hash(current, current + siblings[s]);
		}
	}
	return current == root;
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(MerkleTree)
