#ifndef CLICK_MERKLE_TREE_HH
#define CLICK_MERKLE_TREE_HH

#include <click/element.hh>
#include <click/vector.hh>
#include "../crypto/crypto.hh"

CLICK_DECLS

class MerkleTree {
public:
	typedef unsigned int height_t;

	/**
	 * Creates a Merkle tree from the given values.
	 * Size of input vector needs to be a power of 2.
	 * The leave values of this tree will be the hashed values of the input vector.
	 */
	MerkleTree(const Hash in[], size_t length, const Crypto&);

	/**
	 * Construct a partial Merkle tree
	 */
	MerkleTree(const Hash& root, size_t length, const Crypto&);

	~MerkleTree();

	/**
	 * The the root of the Merke tree
	 */
	inline const Hash& root() const {
		return _flat[0];
	}

	/**
	 * Retrieve all sibling values that are necessary to compute the root value from leaf 'k'
	 */
	void path_to_root(size_t k, Hash path[], height_t max = UINT_MAX) const;

	/**
	 * Check if 'in' is a valid leaf pre-image.
	 *
	 * Returns:
	 * 	0 if 'in' and 'siblings' are valid
	 * -1 if 'in' and 'siblings' are invalid
	 * -2 if unsure since this tree is incomplete and insufficient number of siblings were supplied
	 */
	int valid_leaf(size_t k, const Hash& in, const Hash siblings[], height_t n) const;

	void add(size_t k, const Hash& in, const Hash siblings[], height_t n);

	/**
	 * Verifies that all elements (in, siblings, and root) belong to a valid Merkle tree, i.e. verifies that
	 *
	 *      hash( ... hash(hash(in)||siblings[0])||siblings[1] ...) == root
	 *
	 * 'k' is needed to determine whether siblings[k] is right or left sibling
	 */
	static bool validate(size_t k, const Hash& in, const Hash siblings[], height_t h, const Hash& root, const Crypto& crypto);

	/**
	 * Height of the tree (or height of the root node).
	 *
	 * Height of a node is the longest path from the node to a leaf.
	 * A node
	 */
	inline height_t height() const {
		return _height;
	}

	/**
	 * @return number of leaves
	 */
	inline size_t size() const {
		return _leaves;
	}

private:
	/** used to index flat representation */
	typedef size_t flat_rep_t;

	/* flat representation of Merkle tree */
	Hash* _flat;

	/** height of tree */
	height_t _height;

	/** number of leaves */
	size_t _leaves;

	const Crypto& crypto;

	/** number of nodes at depth 'd' of the tree */
	inline static size_t nodes_at_depth(height_t d) {
		return 1u << d;
	}

	/** index in the flat Merkle tree for node 'off' at depth 'd' */
	inline static flat_rep_t index(height_t d, size_t off = 0) {
		assert(off < nodes_at_depth(d));
		return (1u << d) - 1 + off;
	}

	/** node 'off' on level 'l' */
	inline Hash& element(height_t l, size_t off) {
		return _flat[index(l, off)];
	}

	/** sibling index of node i */
	inline static flat_rep_t sibling(flat_rep_t i) {
		return (i & 1) ? i + 1 : i - 1;
	}

	/** parent index of node 'i' */
	inline static flat_rep_t parent(flat_rep_t i) {
		return (i - 1) >> 1;
	}

	/**
	 * Find base-2 logarithm of a power-2 integer.
	 */
	inline static height_t log2(size_t pow2) {
		height_t log2 = 0;
		while (pow2 >>= 1) log2++;
		return log2;
	}
};

CLICK_ENDDECLS

#endif
