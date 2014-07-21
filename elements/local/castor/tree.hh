#ifndef TREE_HH
#define TREE_HH

#include <click/element.hh>
#include <click/vector.hh>
#include "castor.hh"
#include "crypto.hh"

CLICK_DECLS

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
	static bool isValidMerkleTree(unsigned int id, const SValue& in, const Vector<SValue>& siblings, const SValue& root, const Crypto&);

private:
	class Node;

	Node* _root;
	Vector<Node*> _leaves;
	Crypto& crypto;
};

CLICK_ENDDECLS

#endif
