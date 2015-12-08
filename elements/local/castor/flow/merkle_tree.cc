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
	_leaves.reserve(length);
	for (int i = 0; i < length; i++) {
		Node* leaf = new Node();
		crypto.hash(leaf->data, in[i]);
		_leaves.push_back(leaf);
	}

	// Build the tree
	Vector<Node*> layer = _leaves;
	while (layer.size() > 1) {
		Vector<Node*> nextlayer;
		nextlayer.reserve(layer.size() / 2);

		for (int i = 0; i < layer.size(); i += 2) {
			Node* lc = layer[i];
			Node* rc = layer[i + 1];

			// Compute hash
			Buffer<2*sizeof(Hash)> concat;
			memcpy(concat.data(), lc->data.data(), lc->data.size());
			memcpy(&concat[sizeof(Hash)], rc->data.data(), rc->data.size());

			Node* p = new Node(Hash(), 0, lc, rc);
			crypto.hash(p->data, concat);

			nextlayer.push_back(p);
		}

		// Save new layer
		layer = nextlayer;
	}
	_root = layer[0];
}

MerkleTree::~MerkleTree() {
	// Nodes delete children recursively
	delete _root;
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
	Buffer<2*sizeof(Hash)> current;
	crypto.hash(current.data(), in.data(), in.size());

	for(int s = 0; s < siblings.size(); s++) {
		const Hash& sibling = siblings[s];
		if(i & (1 << s)) { // sibling is left child
			// current = hash(sibling||current)
			memcpy(&current[sizeof(Hash)], &current[0], sizeof(Hash));
			memcpy(&current[0], sibling.data(), sibling.size());
		} else { // sibling is right child
			// current = hash(current||sibling)
			memcpy(&current[sizeof(Hash)], sibling.data(), sibling.size());
		}
		crypto.hash(&current[0], &current[0], current.size());
	}

	// verify that computed root and given root are the same
	int cmp = memcmp(&current[0], root.data(), root.size());
	return cmp == 0;
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(MerkleTree)
