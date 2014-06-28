#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/straccum.hh>
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
	click_chatter("Creating a new Flow Object with %d elements", CASTOR_REAL_FLOWSIZE);
	Flow flow;
	flow.position = 0;

	//Generate some random nonces
	Vector<SValue> ack_auths = Vector<SValue>();	// The ACK authenticator
	Vector<SValue> pids 	= Vector<SValue>();	// Packet IDs

	for(int f=0;f<CASTOR_REAL_FLOWSIZE;f++){
		SValue nonce 	= _crypto->random(CASTOR_HASHLENGTH);
		SValue pid 		= _crypto->hash(nonce);
		ack_auths.push_back(nonce);
		pids.push_back(pid);
	}

	//Build the Merkle Tree
	MerkleTree tree = MerkleTree(pids, _crypto);

	//Set the predefined labels
	for(int i=0;i<CASTOR_REAL_FLOWSIZE;i++){
		SValue root = tree.getRoot();
		PacketLabel lbl;
		memcpy(&lbl.flow_id, root.begin(), CASTOR_HASHLENGTH);
		memcpy(&lbl.packet_id, pids.at(i).begin(),CASTOR_HASHLENGTH);
		// FIXME Missing FlowAuth
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

	if(f->position >= CASTOR_REAL_FLOWSIZE){
		// Delete the Flow
		(_flows.get_pointer(source))->erase(destination);
		//click_chatter("Flow exhausted");
	}
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(TREE)
EXPORT_ELEMENT(CastorFlowMerkle)



