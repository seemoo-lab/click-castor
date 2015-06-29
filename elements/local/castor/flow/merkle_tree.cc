#include <click/config.h>
#include <click/straccum.hh>
#include "merkle_tree.hh"

CLICK_DECLS

class MerkleTree::Node {
public:
	Node(const SValue& data, Node* parent = 0, Node* leftChild = 0, Node* rightChild = 0) :
			data(data), parent(parent), leftChild(leftChild), rightChild(rightChild) {}
	~Node() { delete leftChild;	delete rightChild; }

	bool isRoot() const { return parent == 0; }
	bool isLeaf() const { return leftChild == 0 && rightChild == 0; }
	bool isLeftChild() const { return !isRoot() && (parent->leftChild == this); }
	bool isRightChild() const { return !isRoot() && (parent->rightChild == this); }
	Node* getSibling() const {
		if(isLeftChild())
			return parent->rightChild;
		else if(isRightChild())
			return parent->leftChild;
		else
			return 0;
	}

	const SValue data;
	Node* parent;
	Node* leftChild;
	Node* rightChild;
};

MerkleTree::MerkleTree(const Vector<SValue>& in, const Crypto& c) : crypto(c) {
	if (!(in.size() && !(in.size() & (in.size() - 1)))) {
		click_chatter("Input vector size must be a power of 2, but was %d", in.size());
		return;
	}

	// Create the leaves
	_leaves.reserve(in.size());
	for (int i = 0; i < in.size(); i++) {
		SValue hash = crypto.hash(in[i]);
		Node* leaf = new Node(hash);
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
			SValue concat(lc->data.size() + rc->data.size());
			concat  = lc->data;
			concat += rc->data;
			const SValue hash = crypto.hash(concat);

			// Create a new node
			Node* p = new Node(hash, 0, lc, rc);

			// Update lower layer nodes
			lc->parent = p;
			rc->parent = p;

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

const SValue& MerkleTree::getRoot() const {
	return _root->data;
}

Vector<SValue> MerkleTree::getSiblings(int i) const {
	Vector<SValue> siblings;
	Node* node = _leaves[i];
	while(!node->isRoot()) {
		siblings.push_back(node->getSibling()->data);
		node = node->parent;
	}
	return siblings;
}

bool MerkleTree::isValidMerkleTree(unsigned int i, const SValue& in, const Vector<SValue>& siblings, const SValue& root, const Crypto& crypto) {
	// First hash the input
	SValue current = crypto.hash(in);

	for(int s = 0; s < siblings.size(); s++) {
		SValue sibling = siblings[s];
		if(i & (1 << s)) { // sibling is left child
			// current = hash(sibling||current)
			sibling += current;
			current = crypto.hash(sibling);
		} else { // sibling is right child
			// current = hash(current||sibling)
			current += sibling;
			current = crypto.hash(current);
		}
	}

	assert (root.size() == current.size());
	// verify that computed root and given root are the same
	int cmp = memcmp(current.begin(), root.begin(), root.size());
	return cmp == 0;
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(MerkleTree)
