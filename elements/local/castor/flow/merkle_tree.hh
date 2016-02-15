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
	MerkleTree(const Hash in[], unsigned int length, const Crypto&);
	/**
	 * Construct a partial Merkle tree
	 */
	MerkleTree(const Hash& root, unsigned int length, const Crypto&);
	~MerkleTree();

	/**
	 * The the root of the Merke tree
	 */
	const Hash& root() const;

	/**
	 * Retrieve all sibling values that are necessary to compute the root value from leaf 'k'
	 */
	void path_to_root(unsigned int k, Hash path[]) const;

	bool valid_leaf(unsigned int k, const Hash& in, const Hash siblings[], unsigned int n) const;

	void add(unsigned int k, const Hash& in, const Hash siblings[], unsigned int n);

	/**
	 * Verifies that all elements (in, siblings, and root) belong to a valid Merkle tree, i.e. verifies that
	 *
	 *      hash( ... hash(hash(in)||siblings[0])||siblings[1] ...) == root
	 *
	 * 'k' is needed to determine whether siblings[k] is right or left sibling
	 */
	static bool validate(unsigned int k, const Hash& in, const Hash siblings[], unsigned int h, const Hash& root, const Crypto& crypto);

	/**
	 * Find base-2 logarithm of a power-2 integer efficiently.
	 *
	 * Source: https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
	 */
	static unsigned int log2(unsigned int pow2) {
		static const unsigned int b[] = {0xAAAAAAAA, 0xCCCCCCCC, 0xF0F0F0F0,
		                                 0xFF00FF00, 0xFFFF0000};
		unsigned int r = (pow2 & b[0]) != 0;
		r |= ((pow2 & b[4]) != 0) << 4;
		r |= ((pow2 & b[3]) != 0) << 3;
		r |= ((pow2 & b[2]) != 0) << 2;
		r |= ((pow2 & b[1]) != 0) << 1;
		return r;
	}
private:
	Hash* _flat; /* flat representation of Merkle tree */
	unsigned int height;
	unsigned int leaves;
	const Crypto& crypto;

	/** number of nodes on level 'l' of the tree */
	inline unsigned int nodes_per_level(unsigned int l) const {
		return 1 << l;
	}
	/** index in the flat Merkle tree for node 'off' on level 'l' */
	inline unsigned int index(unsigned int l, unsigned int off = 0) const {
		assert(off < nodes_per_level(l));
		return (1 << l) - 1 + off;
	}
	/** node 'off' on level 'l' */
	inline Hash& element(unsigned int l, unsigned int off) {
		return _flat[index(l, off)];
	}

	/** sibling index of node 'i' */
	inline unsigned int sibling(unsigned int i) const {
		return (i & 1) ? i + 1 : i - 1;
	}
	/** parent index of node 'i' */
	inline unsigned int parent(unsigned int i) const {
		return (i - 1) >> 1;
	}
};

CLICK_ENDDECLS

#endif
