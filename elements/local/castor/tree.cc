#include "tree.hh"
#include "castor.hh"
#include "crypto.hh"
#include <click/straccum.hh>

CLICK_DECLS

MerkleTree::MerkleTree(Vector<SValue> e, Crypto* c) {
	_leaves = Vector<MerkleTreeNode*>();
	_crypto = c;
	Vector<MerkleTreeNode*>* layer;
	Vector<MerkleTreeNode*>* nextlayer = new Vector<MerkleTreeNode*>();

	//Create the leaves
	for(int i=0; i<e.size(); i++){
		//Create a new Node
		MerkleTreeNode* node = new MerkleTreeNode;
		node->parent=0;
		node->data = e.at(i);
		//memcpy(&node->data, e.at(i), sizeof(SHash));
		_leaves.push_back(node);

		//Create a new hashNode
		MerkleTreeNode* hnode = new MerkleTreeNode;
		hnode->parent=0;
		hnode->data = _crypto->hash( e.at(i) );
		//memcpy(&hnode->data, e.at(i), sizeof(Hash));
		//Crypto::hash(&hnode->data, (uint8_t*)&hnode->data, sizeof(Hash));
		node->parent = hnode;
		nextlayer->push_back(hnode);
	}

	// Build the tree
	while(nextlayer->size() > 1){
		layer = nextlayer;
		nextlayer = new Vector<MerkleTreeNode*>();

		for(int j=0; j<layer->size(); j+=2){
			MerkleTreeNode* lc = layer->at(j);
			MerkleTreeNode* rc = layer->at(j+1);

			//Create a new node
			MerkleTreeNode* nn = new MerkleTreeNode;
			nn->parent = 0;

			// Concat the childs values
			SValue tmp = SValue(lc->data);
			for(int k=0;k<rc->data.size();k++)
				tmp.push_back(rc->data.begin()[k]);

			//click_chatter("Hashing data with size");
			//click_chatter(CastorPacket::hexToString(tmp.begin(), tmp.size()).c_str());

			nn->data = _crypto->hash(tmp);


//			Hash nh = Hash();
//			Crypto::hash(&nh->data, (uint8_t*)&data, 2*sizeof(Hash));

			lc->parent=nn;
			rc->parent=nn;

			nextlayer->push_back(nn);

			//click_chatter("Created new element");
			//click_chatter(CastorPacket::hexToString(nn->data.begin(), CASTOR_HASHLENGTH).c_str());
		}
	}
}

MerkleTree::~MerkleTree(){
}

SValue MerkleTree::getRoot(){
	MerkleTreeNode* node = _leaves.front();

	do{
		node = node->parent;
	}while(node->parent != 0);

	return node->data;

	//memcpy(root, node->data, sizeof(SHash));
}

void MerkleTree::getLeaves(Vector<SValue>* leaves){
	leaves->clear();
	for(int i=0; i<_leaves.size(); i++){
		leaves->push_back(_leaves.at(i)->data);
	}
}

void MerkleTree::getHashChain(Vector<SValue>* chain, int id){
	MerkleTreeNode* node = _leaves.at(id);
	chain->clear();

	while(node->parent != 0){
		node = node->parent;
		chain->push_back(node->data);
	}
}



String MerkleTree::toString() {
	StringAccum sa;
	sa << "Generated Merkle Tree:\n";
	for(int i=0; i< _leaves.size(); i++){
		MerkleTreeNode* n = _leaves.at(i);
		do{
			sa << CastorPacket::hexToString(n->data.begin(), n->data.size()) << "->";
			n = n->parent;
		}while(n != 0);
		sa << "\n";
	}
	return sa.take_string();
}



CLICK_ENDDECLS
ELEMENT_PROVIDES(TREE)
