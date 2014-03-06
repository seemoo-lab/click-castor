#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "castor_flow_merkle.hh"
#include "crypto.hh"
#include "tree.hh"

CLICK_DECLS

bool CastorFlowMerkle::hasFlow(Host source, Host destination) {
	HashTable<Host, Flow> * t = _flows.get_pointer(source);
	if(!t) return false;

	Flow* f = t->get_pointer(destination);
	if(!f) return false;

	return true;
}

void CastorFlowMerkle::createFlow(Host source, Host destination) {
	//Create a new Flow Object
	click_chatter("Creating a new Flow Object ");
	Flow flow;
	flow.position = 0;

	//Generate some random nonces
	Vector<SValue> ack_auths = Vector<SValue>();	// The ACK authenticator
	Vector<SValue> pids 	= Vector<SValue>();	// Packet IDs

	//Generate some random nonces
	//Vector<Hash> ack_auths = Vector<Hash>();
	//Vector<Hash> pids = Vector<Hash>();

	//click_chatter("Initialized Vectors");

	for(uint8_t f=0;f<CASTOR_FLOWSIZE;f++){
		SValue nonce 	= _crypto->random(CASTOR_HASHLENGTH);
		SValue pid 		= _crypto->hash(nonce);

		ack_auths.push_back(nonce);
		pids.push_back(pid);
		//Crypto::random()
		//Hash nonce, pid;
//		Crypto::random(&nonce);
//		Crypto::hash(&pid, nonce, sizeof(Hash));
//		ack_auths.push_back(nonce);
//		pids.push_back(pid);
	}

	//Build the Merkle Tree
	MerkleTree tree = MerkleTree(pids, _crypto);
	//click_chatter(tree.toString().c_str());

	//Set the predefined labels
	for(int i=0; i<CASTOR_FLOWSIZE;i++){
		SValue root = tree.getRoot();
		//tree.getRoot(&root);

		PacketLabel lbl;
		//tree.getRoot(&lbl.flow_id);
		memcpy(&lbl.flow_id, root.begin(), CASTOR_HASHLENGTH);
		memcpy(&lbl.packet_id, pids.at(i).begin(),CASTOR_HASHLENGTH);
		memcpy(&lbl.enc_ack_auth,ack_auths.at(i).begin(),CASTOR_HASHLENGTH);
		flow.labels[i] = lbl;
	}

	//Check if a hashtable for the source exists
	if(!_flows.get_pointer(source))
		_flows.set(source, HashTable<Host, Flow>());

	HashTable<Host, Flow>* fdest = _flows.get_pointer(source);
	fdest->set(destination,flow);
}

PacketLabel CastorFlowMerkle::useFlow(Host source, Host destination) {
	Flow* f = (_flows.get_pointer(source))->get_pointer(destination);

	return f->labels[f->position];
}

void CastorFlowMerkle::updateFlow(Host source, Host destination) {
	Flow* f = (_flows.get_pointer(source))->get_pointer(destination);

	f->position++;

	if(f->position >= CASTOR_FLOWSIZE){
		// Delete the Flow
		(_flows.get_pointer(source))->erase(destination);
		//click_chatter("Flow exhausted");
	}
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(TREE)
ELEMENT_LIBS(-L/usr/local/lib -lbotan-1.10)
EXPORT_ELEMENT(CastorFlowMerkle)



