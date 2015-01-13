#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "crypto.hh"

#include <botan/hash.h>
#include <botan/auto_rng.h>
#include <botan/symkey.h>
#include <botan/pipe.h>

CLICK_DECLS

Crypto::Crypto() {
	sam = 0;
	hashFunction = 0;
}

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

void Crypto::hash(Hash hash, const uint8_t* data, uint8_t length) const {
	hashFunction->clear();
	Botan::SecureVector<Botan::byte> hashed = hashFunction->process(data, length);
	memcpy(hash, hashed.begin(), sizeof(Hash));
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
const SymmetricKey* Crypto::getSharedKey(IPAddress address) const {
	const SecurityAssociation* sharedKeySA = sam->getSA(SAsharedsecret, address);
	if (!sharedKeySA) {
		return 0;
	}
	// TODO Use Botan::OctetString directly in SecurityAssociation to avoid creating a new instance every time
	SymmetricKey* sharedKey = new Botan::OctetString(sharedKeySA->myData, sharedKeySA->mySize);
	return sharedKey;
}

/**
 * Encrypt plain using AES-128 in CTS mode.
 */
SValue Crypto::encrypt(const SValue& plain, const SymmetricKey& key) const {
	Botan::Pipe encryptor(get_cipher(algo, key, iv, Botan::ENCRYPTION));
	encryptor.process_msg(plain);
	return encryptor.read_all();
}

/**
 * Decrypt cipher using the given key. Note that you might have to remove padding that was added during encryption.
 */
SValue Crypto::decrypt(const SValue& cipher, const SymmetricKey& key) const {
	Botan::Pipe decryptor(get_cipher(algo, key, iv, Botan::DECRYPTION));
	decryptor.process_msg(cipher);
	return decryptor.read_all();
}

SValue Crypto::hash(const SValue& data) const {
	return hashFunction->process(data);
}

void Crypto::testSymmetricCrypt(SValue plain, IPAddress dst) const {
	const SymmetricKey* key = getSharedKey(dst);

	if (!key) {
		click_chatter("Encryption error, no key for %s found", dst.unparse().c_str());
		return;
	}

	SValue encrypted = encrypt(plain, *key);
	SValue decrypted = decrypt(encrypted, *key);
	click_chatter("Plaintext: \t%s", CastorPacket::hexToString(plain.begin(), plain.size()).c_str());
	click_chatter("Encrypt: \t%s",
			CastorPacket::hexToString(encrypted.begin(), encrypted.size()).c_str());
	click_chatter("Decrypt: \t%s",
			CastorPacket::hexToString(decrypted.begin(), decrypted.size()).c_str());

	if (plain.operator ==(SValue(decrypted.begin(), plain.size()))) {
		click_chatter("Encryption successful");
	} else {
		click_chatter("Encryption failed");
	}
}

CLICK_ENDDECLS
ELEMENT_LIBS(-L/usr/local/lib -lbotan-1.10)
EXPORT_ELEMENT(Crypto)
