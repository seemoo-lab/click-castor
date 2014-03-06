#ifndef TREE_HH
#define TREE_HH
#include <click/config.h>
#include <click/confparse.hh>
#include <click/element.hh>
#include "crypto.hh"
#include <click/vector.hh>


typedef struct MerkleTreeNode MerkleTreeNode;
struct MerkleTreeNode{
	MerkleTreeNode* parent;
	SValue			data;
};


class MerkleTree{
public:
	MerkleTree(Vector<SValue> e, Crypto* c);
	~MerkleTree();

	SValue 			getRoot();
    void 			getLeaves(Vector<SValue>*);
	void 			getHashChain(Vector<SValue>*, int id);
	String 			toString();

private:
	Vector<MerkleTreeNode*> _leaves;
	Crypto* _crypto;
};
#endif
