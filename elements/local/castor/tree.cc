#include "tree.hh"
#include "castor.hh"
#include "crypto.hh"
#include <click/straccum.hh>

CLICK_DECLS

///**
// * Creates a new TreeNode with given value and parent set to zero
// */
//MerkleTreeNode::MerkleTreeNode(Hash* value) {
//	memcpy(&_value,value,sizeof(Hash));
//	_parent = 0;
//}
//
///**
// * Creates a new TreeNode as parent for a two nodes
// */
//MerkleTreeNode::MerkleTreeNode(MerkleTreeNode* leftnode, MerkleTreeNode* rightnode){
//	//Hash the values of the childs to create a new Node
//	Hash lvalue, rvalue;
//	leftnode->getValue(&_value);
//	rightnode->getValue(&rvalue);
//	//_value = lvalue;
//	leftnode->setParent(this);
//	rightnode->setParent(this);
//	_parent = 0;
//}
//
///**
// * Creates a new TreeNode as parent only one subnode
// */
//MerkleTreeNode::MerkleTreeNode(MerkleTreeNode* subnode){
//	Hash subvalue;
//	subnode->getValue(&_value);
//	//_value = subvalue;
//	subnode->setParent(this);
//	_parent = 0;
//}
//
//
//MerkleTreeNode::~MerkleTreeNode() {
//}
//
///**
// * Sets the parent pointer to the newParent object,
// * returns the old parent
// */
//MerkleTreeNode* MerkleTreeNode::setParent(MerkleTreeNode* newParent){
//	MerkleTreeNode* old = _parent;
//	_parent = newParent;
//	return old;
//}
//
//MerkleTreeNode* MerkleTreeNode::getParent(){
//	return _parent;
//}
//
///**
// * Return the content of the node
// */
//void MerkleTreeNode::getValue(Hash* value){
//	memcpy(value, &_value, sizeof(Hash));
//}
//
//
//MerkleTree::MerkleTree(Vector<Hash> e) {
//	_leaves = Vector<MerkleTreeNode*>();
//	_tree = Vector<MerkleTreeNode*>();
//
//	//Create the leaves
//	for(int i=0; i<e.size(); i++){
//		Hash h;
//		memcpy(&h,e.at(i), sizeof(Hash));
//		MerkleTreeNode node = new MerkleTreeNode(&h);
//		_leaves.push_back(node);
//
//		//Add a hashed parent
//		MerkleTreeNode par = new MerkleTreeNode(node);
//		_tree.push_back(par);
//
//	}
//
//	//Create the first hashes
//
//
//
////	_tree = Vector<TreeLayer>();
////
////	//Create the Leaves
////	TreeLayer dataleaves = TreeLayer();
////	for(int i=0; i<e.size(); i++){
////		Hash h;
////		memcpy(&h,e.at(i), sizeof(Hash));
////		//Create a new MerkleTreeNode Object
////		MerkleTreeNode node = new MerkleTreeNode(&h);
////		dataleaves.push_back(node);
////	}
////	_tree.push_back(dataleaves);
////
////	//Create the second layer
////	TreeLayer leaves = TreeLayer();
////	for(int j=0; j<_tree.back().size(); j++){
////		MerkleTreeNode n = _tree.back().at(j);
////		MerkleTreeNode node = new MerkleTreeNode(n);
////		leaves.push_back(node);
////	}
////	_tree.push_back(leaves);
////
////	do{
////		TreeLayer layer = TreeLayer();
////		for(int k=0; k<_tree.back().size(); k+=2){
////			MerkleTreeNode n1 	= _tree.back().at(k);
////			MerkleTreeNode n2 	= _tree.back().at(k+1);
////			MerkleTreeNode nn 	= new MerkleTreeNode(&n1, &n2);
////			layer.push_back(nn);
////		}
////		_tree.push_back(layer);
////
////		//Repeat until only one element is left
////	}while(_tree.back().size() > 1);
//}
//
//MerkleTree::~MerkleTree() {
//}
//
////void MerkleTree::getRoot(Hash* hash) {
////	return _tree.back().back().getValue(hash);
////}
//
////Vector<Hash> MerkleTree::getHashChain() {
////	Vector<Hash> chain = Vector<Hash>();
////
////	//Pick the start node
////	MerkleTreeNode* node = &_tree.front().front();
////
////	do{
////		Hash h;
////		node->getValue(&h);
////		chain.push_back(h);
////		node = node->getParent();
////	}while(node != 0);
////
////	return chain;
////}
//
//MerkleTreeNode* MerkleTree::getParent(MerkleTreeNode* val) {
//	return val->getParent();
//}
//
//String MerkleTree::toString() {
//	StringAccum sa;
//	sa << "Generated Merkle Tree:\n";
//	//for(int i=0; i<_tree.size();i++){
//	//	TreeLayer layer = _tree.at(i);
//		for(int j=0; j<_leaves.size(); j++){
//			MerkleTreeNode node = _leaves.at(j);
//			Hash value;
//			node.getValue(&value);
//			sa << CastorPacket::hexToString(value,sizeof(Hash)) << " ";
//		}
//		sa << "\n";
//
//		for(int j=0; j<_leaves.size(); j++){
//			MerkleTreeNode* node = _leaves.at(j)->getParent();
//			Hash value;
//			node->getValue(&value);
//			sa << CastorPacket::hexToString(value,sizeof(Hash)) << " ";
//		}
//		sa << "\n";
//	//}
//
////	sa << "First path: ";
////	Vector<Hash> hchain = getHashChain();
////	for(int j=0; j<hchain.size(); j++){
////		sa<< CastorPacket::hexToString(hchain.at(j),sizeof(Hash)) << " -> ";
////	}
////
////	sa << "\nRoot: ";
////	Hash root;
////	getRoot(&root);
////	sa << CastorPacket::hexToString(root,sizeof(Hash));
//
//	return sa.take_string();
//}

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


			//tmp.copy(lc->data.begin(), lc->data.end());
			//tmp.copy(rc->data.begin(), rc->data.end());

//			//tmp.SecureVector(lc->data., rc->data);
//			tmp.append(lc->data.begin(),lc->data.size());
//			tmp.append(rc->data.begin(),rc->data.size());

//			struct doubleHash{
//				Hash h1;
//				Hash h2;
//			} data;
//
//			memcpy(&data.h1, lc->data.begin(), sizeof(Hash));
//			memcpy(&data.h2, rc->data.begin(), sizeof(Hash));

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
