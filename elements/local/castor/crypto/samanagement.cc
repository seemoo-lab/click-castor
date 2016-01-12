#include <click/config.h>
#include <click/args.hh>
#include "samanagement.hh"

CLICK_DECLS

int SAManagement::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("ADDR", myAddr)
			.read_or_set_p("SYM_KEY_LENGTH", symmetricKeyLength, 32)
			.complete();
}

void SAManagement::add(const NodeId& node, const SecurityAssociation& sa) {
	sas[node].push_back(sa);
}

const SecurityAssociation* SAManagement::get(const NodeId& node, SecurityAssociation::Type type) {
	if (sas.count(node) > 0) {
		for (const auto& sa : sas[node])
			if (sa.type == type)
				return &sa;
	}

	// If no entry currently exists, generate one
	if (type == SecurityAssociation::sharedsecret) {
		add(node, genereateSymmetricSA(node));
		return get(node, type);
	}

	return 0;
}

SecurityAssociation SAManagement::genereateSymmetricSA(const NodeId& node) {
	Vector<uint8_t> key;
	key.reserve(symmetricKeyLength);
	if (myAddr.addr() < node.addr()) {
		for (int i = 0; i < sizeof(NodeId); i++)
			key.push_back(myAddr.data()[i]);
		for (int i = 0; i < sizeof(NodeId); i++)
			key.push_back(node.data()[i]);
	} else {
		for (int i = 0; i < sizeof(NodeId); i++)
			key.push_back(node.data()[i]);
		for (int i = 0; i < sizeof(NodeId); i++)
			key.push_back(myAddr.data()[i]);
	}
	while (key.size() < symmetricKeyLength)
		key.push_back(0);
	return SecurityAssociation(SecurityAssociation::sharedsecret, key);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(SAManagement)
