/*
 * SAManagement.{cc,hh} -- Manage Security Associations
 * Joerg Kaiser
 *
 */
#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "samanagement.hh"
#include <botan/kdf.h>
#include <botan/lookup.h>
#include <botan/secmem.h>

CLICK_DECLS

SAManagement::SAManagement(size_t symmetricKeyLength) :
		symmetricKeyLength(symmetricKeyLength) {
}

SAManagement::~SAManagement() {
}

int SAManagement::configure(Vector<String> &conf, ErrorHandler *errh) {
	// Set default values
	symmetricKeyLength = 16;

	if (Args(conf, this, errh)
			.read_mp("ADDR", myIP)
			.read_p("SYM_KEY_LENGTH", symmetricKeyLength)
			.complete() < 0)
		return -1;

	return 0;
}

int SAManagement::initialize(ErrorHandler* errh) {

	int ret = initializeSymmetricKeys(errh);
	if (ret != 0)
		return ret;

	ret = initializePublicPrivateKeys(errh);
	if (ret != 0)
		return ret;

	return 0;

}

int SAManagement::initializeSymmetricKeys(ErrorHandler*) {

	// Create constant base key
	Botan::byte rbytes[symmetricKeyLength];
	for (unsigned int i = 0; i < symmetricKeyLength; i++) {
		rbytes[i] = i;
	}

	// Derive individual keys deterministically for every node
	Botan::KDF* kdf = Botan::get_kdf("KDF2(SHA-160)");
	unsigned int numOfNodes = 50; // max: 254
	String net = "192.168.201.";
	for (unsigned char i = 1; i <= numOfNodes; i++) {
		String addr(net);
		addr.append(String(i));
		Botan::SecureVector<Botan::byte> key = kdf->derive_key(
				symmetricKeyLength, rbytes, symmetricKeyLength,
				(Botan::byte*) addr.data(), addr.length());
		assert(key.size() == symmetricKeyLength);
		SecurityAssociation sa(SAsharedsecret, key.begin(), key.size());

		addSA(sa, IPAddress(addr));
	}

	delete kdf;

	return 0;
}

int SAManagement::initializePublicPrivateKeys(ErrorHandler*) {
	String strpublickey =
			"-----BEGIN PUBLIC KEY-----MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDgCtq3o51JBkJyLagmnMovOPqebMsHy5Z0CYyHQUyPGV+6k4oWLFVWmoSGDkmcanpsGtaqKHERqfWY38H1Bz7U99mRmrWU3nfYwQqIz+TZkGyOVbzoTeiZ3ApqvUwIbhYJ3zRElmaPajRUdHolrBKgfAqBpvlCtPIwof48D/OeBwIDAQAB-----END PUBLIC KEY-----";

	SecurityAssociation sa1(SApubkey, strpublickey);

	String strprivatekey =
			"-----BEGIN ENCRYPTED PRIVATE KEY-----\
	MIIC0jBMBgkqhkiG9w0BBQ0wPzAeBgkqhkiG9w0BBQwwEQQICpOWuM+j/gACAicQ\
	AgEgMB0GCWCGSAFlAwQBKgQQ+/8chioJV2Dj1uoCVrAN9wSCAoDxhzkndZsQakKL\
	ux4ZtcfMTUvr1MPxAGDfBV9FknXvib8ple5RmbvlVX+OT7Pak1ly7mjeg0afVBHd\
	BI98J3yJv79StA+QeRGFckK2hFPgMkX/lJevN32qfNMCJBLnxxkaO9vwC56coAJ9\
	JWcTN34Trgc0Y61KzkcxNUpBhABO6jJSmmjzHtQyyjF+M54JA08Vg1417uZESe3o\
	FkyX0qpUTBkA69qOA0I3n7zELte6YFgogKDSwenFOo8APMIboGAvc7xazWMt3Dqm\
	CIYxqP4Ot9lhLn1UwEM8oN1USTQG1ijpVROPYa4abd5n1YzRcLE84NKR9ScRDv/G\
	dkiSa9zg1eJs72trs1ttz4saNTb7VXd5gwoKhwRQOcnqt3dcXJK9p2GcY8LlswHb\
	9L3oqdQtbZ1W8iJEGzdvP08BuDXGcXX2Dy+3/EXQwxpZPfinIfeLHIV9hvyDkOHx\
	yBRejmZl0OxFiQn6WEiiEPpn94L8CzR2UaB4koUM8rMgYP/3LTRThT4JnsQlargI\
	1kTNTdG/L2Ya3HiCEHM0axSXUvUeITru0TVTk+lg6/SKPIMHz6VHp2iwVN7OxeNe\
	Gok1JF0l5BFPJOiamVjpPveEJGRfXA5y7KAliYyNKnsC3UxfT250BxXpf7HcheZ7\
	t5e6fiFzhZm84DEZPi8cYixfKhUTZ55kcAwkUP3sNavWB4dihNEHET9WodiZFUjB\
	OMmvwYWHGeTAHuwdNva30tpkjFqGMvkFNu3BhGUPJZocp5iK22P2KS7k2Bd7dtrt\
	vWe+Muos3RLIZ8afSDtecl9O+CsHgnyGLn8thmv+XnNVmo+ta5kT8l6r/z34rD8S\
	cUQQuSKm\
	-----END ENCRYPTED PRIVATE KEY-----";

	SecurityAssociation sa2(SAprivkey, strprivatekey);

	addSA(sa2, myIP);

	addSA(sa1, IPAddress("192.168.201.1"));
	addSA(sa1, IPAddress("192.168.201.2"));
	addSA(sa1, IPAddress("192.168.201.3"));
	addSA(sa1, IPAddress("192.168.201.4"));
	addSA(sa1, IPAddress("192.168.201.5"));
	addSA(sa1, IPAddress("192.168.201.6"));
	addSA(sa1, IPAddress("192.168.201.7"));
	addSA(sa1, IPAddress("192.168.201.8"));
	addSA(sa1, IPAddress("192.168.201.9"));
	addSA(sa1, IPAddress("192.168.201.10"));
	addSA(sa1, IPAddress("192.168.201.11"));
	addSA(sa1, IPAddress("192.168.201.12"));
	addSA(sa1, IPAddress("192.168.201.13"));
	addSA(sa1, IPAddress("192.168.201.14"));
	addSA(sa1, IPAddress("192.168.201.15"));
	addSA(sa1, IPAddress("192.168.201.16"));
	addSA(sa1, IPAddress("192.168.201.17"));
	addSA(sa1, IPAddress("192.168.201.18"));
	addSA(sa1, IPAddress("192.168.201.19"));
	addSA(sa1, IPAddress("192.168.201.20"));
	return 0;
}

int SAManagement::addSA(SecurityAssociation sa, IPAddress node) {
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
	for (SAMap::iterator i = mySAs.begin(); i != mySAs.end(); i++) {
		for (int k = 0; k < i.value().size(); k++) {
			click_chatter("we have a SA of type %i for %s",
					(i.value())[k].myType, i.key().s().c_str());
		}
	}
}

bool SAManagement::checkSApresence(SAType t, IPAddress node) {
	click_chatter("searching for SA of type %i for node %s", t,
			node.s().c_str());
	SAMap::iterator it = mySAs.find(node);
	if (it != mySAs.end()) {
		for (int k = 0; k < it.value().size(); k++) {
			if ((it.value())[k].myType == t) {
				click_chatter("data is %s", (it.value())[k].toString().c_str());
				return true;
			}
		}
		return false;
	} else {
		return false;
	}
}

const SecurityAssociation* SAManagement::getSA(SAType t,
		const IPAddress& node) {
	SAMap::iterator it = mySAs.find(node);
	if (it != mySAs.end()) {
		for (int k = 0; k < it.value().size(); k++) {
			if ((it.value())[k].myType == t) {
				return &(it.value())[k];
			}
		}
		return 0;
	} else {
		return 0;
	}
}

CLICK_ENDDECLS
ELEMENT_LIBS(-L/usr/local/lib -lbotan-1.10)
EXPORT_ELEMENT(SAManagement)
