#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "samanagement.hh"
#include <botan/kdf.h>
#include <botan/lookup.h>
#include <botan/secmem.h>

CLICK_DECLS

int SAManagement::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("ADDR", myAddr)
			.read_p("SYM_KEY_LENGTH", symmetricKeyLength)
			.complete();
}

int SAManagement::addSA(SecurityAssociation sa, const NodeId& node) {
	SAMap::iterator it = mySAs.find(node);
	if (it != mySAs.end()) {
		it.value().push_back(sa);
	} else {
		Vector<SecurityAssociation> newentry;
		newentry.push_back(sa);
		mySAs.set(node, newentry);
	}
	return 0;
}

void SAManagement::printSAs() {
	for (SAMap::iterator i = mySAs.begin(); i != mySAs.end(); i++)
		for (int k = 0; k < i.value().size(); k++)
			click_chatter("SA of type %i for %s", (i.value())[k].myType, i.key().unparse().c_str());
}

bool SAManagement::checkSApresence(SAType t, const NodeId& node) {
	SAMap::iterator it = mySAs.find(node);
	if (it == mySAs.end())
		return false;
	SAs& sas = it.value();
	for (SecurityAssociation* sa = sas.begin(); sa != sas.end(); ++sa)
		if (sa->myType == t)
			return true;
	return false;
}

const SecurityAssociation* SAManagement::getSA(SAType t, const NodeId& node) {
	if (t == SAsharedsecret && !checkSApresence(t, node))
		addSA(genereateSymmetricSA(node), node);

	SAMap::iterator it = mySAs.find(node);
	if (it == mySAs.end())
		return 0;
	SAs& sas = it.value();
	for (SecurityAssociation* sa = sas.begin(); sa != sas.end(); ++sa)
		if (sa->myType == t)
			return sa;
	return 0;
}

SecurityAssociation SAManagement::genereateSymmetricSA(const NodeId& node) {
	// Create constant base key
	Botan::byte rbytes[symmetricKeyLength];
	for (unsigned int i = 0; i < symmetricKeyLength; i++) {
		rbytes[i] = i;
	}
	// Create salt based on the NodeIds of both parties
	Botan::byte salt[sizeof(NodeId) * 2];
	if (myAddr.addr() < node.addr()) {
		memcpy(salt, myAddr.data(), sizeof(NodeId));
		memcpy((salt + sizeof(NodeId)), node.data(), sizeof(NodeId));
	} else {
		memcpy((salt + sizeof(NodeId)), myAddr.data(), sizeof(NodeId));
		memcpy(salt, node.data(), sizeof(NodeId));
	}
	// Derive the shared key
	Botan::KDF* kdf = Botan::get_kdf("KDF2(SHA-160)");
	Botan::SecureVector<Botan::byte> key = kdf->derive_key(
			symmetricKeyLength, rbytes, sizeof(rbytes), salt,
			sizeof(salt));
	return SecurityAssociation(SAsharedsecret, key.begin(), key.size());
}

CLICK_ENDDECLS
ELEMENT_LIBS(-L/usr/local/lib -lbotan-1.10)
EXPORT_ELEMENT(SAManagement)
