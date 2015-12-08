#include <click/config.h>
#include <click/args.hh>
#include "samanagement.hh"
#include <botan/kdf.h>
#include <botan/lookup.h>
#include <botan/secmem.h>

CLICK_DECLS

int SAManagement::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("ADDR", myAddr)
			.read_or_set_p("SYM_KEY_LENGTH", symmetricKeyLength, 16)
			.complete();
}

void SAManagement::add(const NodeId& node, const SecurityAssociation& sa) {
	sas[node].push_back(sa);
}

const SecurityAssociation* SAManagement::get(const NodeId& node, SecurityAssociation::Type type) {
	const SecurityAssociation* sa = NULL;
	SAMap::iterator it = sas.find(node);
	if (it != sas.end()) {
		SAs& sas = it.value();
		for (const auto& sa : sas)
			if (sa.type == type)
				return &sa;
	}

	// If no entry currently exists, generate one
	if (sa == NULL && type == SecurityAssociation::sharedsecret) {
		add(node, genereateSymmetricSA(node));
		return get(node, type);
	}

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
	return SecurityAssociation(SecurityAssociation::sharedsecret, key);
}

CLICK_ENDDECLS
ELEMENT_LIBS(-L/usr/local/lib -lbotan-1.10)
EXPORT_ELEMENT(SAManagement)
