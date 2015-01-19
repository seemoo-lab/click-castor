#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/straccum.hh>
#include "castor_flow_merkle.hh"
#include "tree.hh"

CLICK_DECLS

bool CastorFlowMerkle::hasFlow(Host source, Host destination) {
	HashTable<Host, Flow> * t = _flows.get_pointer(source);
	if (!t)
		return false;

	Flow* f = t->get_pointer(destination);
	if (!f)
		return false;

	return true;
}

void CastorFlowMerkle::createFlow(Host source, Host destination) {
	// Create a new Flow Object
//	click_chatter("Creating a new Flow object (%s -> %s) with %d elements",
//			source.unparse().c_str(), destination.unparse().c_str(),
//			CASTOR_FLOWSIZE);
	Flow flow;
	flow.position = 0;

	// Generate random nonces
	Vector<SValue> ack_auths = Vector<SValue>(); // The ACK authenticator
	Vector<SValue> pids = Vector<SValue>();	// Packet IDs

	for (int f = 0; f < CASTOR_FLOWSIZE; f++) {
		SValue nonce = _crypto->random(sizeof(ACKAuth));
		SValue pid = _crypto->hash(nonce);
		ack_auths.push_back(nonce);
		pids.push_back(pid);
	}

	// Create labels
	MerkleTree tree = MerkleTree(pids, *_crypto);

	for (unsigned int i = 0; i < CASTOR_FLOWSIZE; i++) {
		PacketLabel lbl;
		lbl.packet_number = i;

		// Set flow id
		SValue root = tree.getRoot();
		Hash fid(root.begin());
		lbl.flow_id = Hash(root.begin());

		// Set packet id
		lbl.packet_id = Hash(pids.at(i).begin());

		// Set flow authenticator
		Vector<SValue> siblings;
		tree.getSiblings(siblings, i);
		assert(siblings.size() == CASTOR_FLOWAUTH_ELEM);
		for (int j = 0; j < siblings.size(); j++)
			lbl.flow_auth[j].data = Hash(siblings.at(j).begin());

		// Set unencrypted (!) ACK authenticator
		lbl.ack_auth = Hash(ack_auths.at(i).begin());
		flow.labels[i] = lbl;
	}

	//Check if a hashtable for the source exists
	if (!_flows.get_pointer(source))
		_flows.set(source, HashTable<Host, Flow>());

	HashTable<Host, Flow>* fdest = _flows.get_pointer(source);
	fdest->set(destination, flow);
}

PacketLabel CastorFlowMerkle::useFlow(Host source, Host destination) {
	Flow* f = (_flows.get_pointer(source))->get_pointer(destination);
	return f->labels[f->position];
}

void CastorFlowMerkle::updateFlow(Host source, Host destination) {
	Flow* f = (_flows.get_pointer(source))->get_pointer(destination);

	f->position++;

	if (f->position >= CASTOR_FLOWSIZE) {
		(_flows.get_pointer(source))->erase(destination);
	}
}

CLICK_ENDDECLS
ELEMENT_REQUIRES(TREE)
EXPORT_ELEMENT(CastorFlowMerkle)
