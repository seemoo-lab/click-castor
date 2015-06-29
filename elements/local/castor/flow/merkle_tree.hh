#ifndef CLICK_MERKLE_TREE_HH
#define CLICK_MERKLE_TREE_HH

#include <click/element.hh>
#include <click/vector.hh>
#include "../crypto/crypto.hh"

CLICK_DECLS

class MerkleTree {
public:
	/**
	 * Creates a Merkle tree from the given values.
	 * Size of input vector needs to be a power of 2.
	 * The leave values of this tree will be the hashed values of the input vector.
	 */
	MerkleTree(const Vector<SValue>&, const Crypto&);
	~MerkleTree();

	/**
	 * The the root of the Merke tree
	 */
	const SValue& getRoot() const;

	/**
	 * Retrieve all sibling values that are necessary to compute the root value from leaf i
	 */
	Vector<SValue> getSiblings(int i) const;

	/**
	 * Verifies that all elements (in, siblings, and root) belong to a valid Merkle tree, i.e. verifies that
	 *
	 *      hash( ... hash(hash(in)||siblings[0])||siblings[1] ...) == root
	 *
	 * i is needed to determine whether siblings[i] is right or left sibling
	 */
	static bool isValidMerkleTree(unsigned int i, const SValue& in, const Vector<SValue>& siblings, const SValue& root, const Crypto& crypto);

private:
	class Node;

	const Node* _root;
	Vector<Node*> _leaves;
	const Crypto& crypto;
};

CLICK_ENDDECLS

#endif
