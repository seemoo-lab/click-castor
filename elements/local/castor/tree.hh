#ifndef TREE_HH
#define TREE_HH

#include <click/element.hh>
#include <click/vector.hh>
#include "castor.hh"
#include "crypto.hh"

CLICK_DECLS

typedef struct treenode MerkleTreeNode;
struct treenode {
	MerkleTreeNode* parent;
	MerkleTreeNode* leftChild;
	MerkleTreeNode* rightChild;
	SValue data;
	treenode(MerkleTreeNode* parent = 0, MerkleTreeNode* leftChild = 0, MerkleTreeNode* rightChild = 0) : parent(parent), leftChild(leftChild), rightChild(rightChild) {}
	~treenode() {}
	bool isRoot() { return parent == 0; }
	bool isLeaf() {	return leftChild == 0 && rightChild == 0; }
	bool isLeftChild() { return !isRoot() && (parent->leftChild == this); }
	bool isRightChild() { return !isRoot() && (parent->rightChild == this); }
	MerkleTreeNode* getSibling() { return (isLeftChild() ? parent->rightChild : (isRightChild() ? parent->leftChild : 0)); }
};

class MerkleTree {
public:

	/**
	 * Creates a Merkle tree from the given values.
	 * Size of input vector needs to be a power of 2.
	 * The leave values of this tree will be the hashed values of the input vector.
	 */
	MerkleTree(Vector<SValue>&, Crypto&);
	~MerkleTree();

	/**
	 * The the root of the Merke tree
	 */
	SValue getRoot();

	/**
	 * Append all leave values to the given Vector
	 */
	void getLeaves(Vector<SValue>&);

	/**
	 * Retrieve all sibling values that are necessary to compute the root value from id
	 */
	void getSiblings(Vector<SValue>& siblings, int id);
	String toString();

	/**
	 * Verifies that all elements (in, siblings and root) belong to a valid Merkle tree, i.e. verifies that
	 *
	 *      hash( ... hash(hash(in)||siblings[0])||siblings[1] ...) == root
	 *
	 * id is needed to determine whether siblings[i] is right or left sibling
	 */
	static bool isValidMerkleTree(unsigned int id, SValue& in, Vector<SValue>& siblings, SValue& root, Crypto&);

private:
	MerkleTreeNode* _root;
	Vector<MerkleTreeNode*> _leaves;
	Crypto& crypto;
};

CLICK_ENDDECLS

#endif
