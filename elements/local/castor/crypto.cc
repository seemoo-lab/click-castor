#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "crypto.hh"

#include <botan/hash.h>
#include <botan/sha160.h>
#include <botan/auto_rng.h>
#include <botan/rsa.h>
#include <botan/pubkey.h>
#include <botan/symkey.h>
#include <botan/aes.h>

CLICK_DECLS

Crypto::Crypto() {
}

Crypto::~Crypto() {
}

/*
 * Configure the Crypto Element
 */
int Crypto::configure(Vector<String> &conf, ErrorHandler *errh)
{
	int res = cp_va_kparse(conf, this, errh,
		"SAM", cpkP+cpkM, cpElementCast, "SAManagement", &_sam,
		cpEnd);
	if(res < 0) return res;

	blockCipher = new Botan::AES_128();

	return 0;
}

void Crypto::hash(Hash hash, const uint8_t* data, uint8_t length) const {
	Botan::SHA_160 sha160;
	Botan::SecureVector<Botan::byte> hashed = sha160.process(data, length);
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
	const SecurityAssociation* sharedKeySA = _sam->getSA(SAsharedsecret, address);
	if (!sharedKeySA) {
		return 0;
	}
	SymmetricKey* sharedKey = new Botan::OctetString(sharedKeySA->myData, sharedKeySA->mySize);
	return sharedKey;
}

/**
 * Encrypt plain using a block cipher. Add padding if length of plain is not multiple of block size.
 */
SValue Crypto::encrypt(const SValue& plain, const SymmetricKey& key) const {
	blockCipher->set_key(key);

	size_t numBlocks = numberOfBlocks(blockCipher->block_size(), plain.size());
	size_t cipherLength = numBlocks * blockCipher->block_size();

	Botan::byte block[cipherLength];
	memcpy(block, plain.begin(), plain.size());
	blockCipher->encrypt_n(block, block, numBlocks);

	return SValue(block, cipherLength);
}

/**
 * Decrypt cipher using the given key. Note that you might have to remove padding that was added during encryption.
 */
SValue Crypto::decrypt(const SValue& cipher, const SymmetricKey& key) const {
	blockCipher->set_key(key);

	size_t numBlocks = numberOfBlocks(blockCipher->block_size(), cipher.size());
	size_t cipherLength = numBlocks * blockCipher->block_size();

	Botan::byte block[cipherLength];
	memcpy(block, cipher.begin(), cipher.size());
	blockCipher->decrypt_n(block, block, numBlocks);

	return SValue(block, cipherLength);
}

SValue Crypto::hash(const SValue& data) const {
	Botan::SHA_160 sha160;
	SValue hashed = sha160.process(data.begin(), data.size());
	return hashed;
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
