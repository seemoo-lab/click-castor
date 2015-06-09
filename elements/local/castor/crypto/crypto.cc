#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "crypto.hh"

#include <botan/hash.h>
#include <botan/auto_rng.h>
#include <botan/symkey.h>
#include <botan/pipe.h>

CLICK_DECLS

Crypto::~Crypto() {
	delete hashFunction;
}

/*
 * Configure the Crypto Element
 */
int Crypto::configure(Vector<String> &conf, ErrorHandler *errh)
{
	int res = cp_va_kparse(conf, this, errh,
		"SAM", cpkP+cpkM, cpElementCast, "SAManagement", &sam,
		cpEnd);
	if(res < 0) return res;

	algo = "AES-128/CBC/CTS";
	iv = Botan::InitializationVector("00000000000000000000000000000000");
	hashFunction = Botan::get_hash("SHA-160");

	return 0;
}

SValue Crypto::random(int bytes) const {
	Botan::AutoSeeded_RNG rng;
	Botan::byte rbytes[bytes];
	rng.randomize(rbytes, bytes);
	return SValue(rbytes, bytes);
}

/**
 * Return the symmetric shared key for a destination
 */
const SymmetricKey* Crypto::getSharedKey(NodeId id) const {
	const SecurityAssociation* sharedKeySA = sam->get(id, SecurityAssociation::sharedsecret);
	if (!sharedKeySA)
		return 0;
	const SymmetricKey* sharedKey = &sharedKeySA->secret;
	return sharedKey;
}

/**
 * Encrypt plain using AES-128 in CTS mode.
 */
SValue Crypto::encrypt(const SValue& plain, const SymmetricKey& key) const {
	Botan::Pipe encryptor(get_cipher(algo.c_str(), key, iv, Botan::ENCRYPTION));
	encryptor.process_msg(plain);
	return encryptor.read_all();
}

/**
 * Decrypt cipher using the given key. Note that you might have to remove padding that was added during encryption.
 */
SValue Crypto::decrypt(const SValue& cipher, const SymmetricKey& key) const {
	Botan::Pipe decryptor(get_cipher(algo.c_str(), key, iv, Botan::DECRYPTION));
	decryptor.process_msg(cipher);
	return decryptor.read_all();
}

SValue Crypto::hash(const SValue& data) const {
	return hashFunction->process(data);
}

CLICK_ENDDECLS
ELEMENT_LIBS(-L/usr/local/lib -lbotan-1.10)
EXPORT_ELEMENT(Crypto)
