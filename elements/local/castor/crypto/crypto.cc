#include <click/config.h>
#include <click/args.hh>
#include "crypto.hh"

#include <sodium.h>
#include <botan/hash.h>
#include <botan/auto_rng.h>
#include <botan/symkey.h>
#include <botan/pipe.h>

CLICK_DECLS

int Crypto::configure(Vector<String> &conf, ErrorHandler *errh)
{
	if (Args(conf, this, errh)
			.read_mp("SAM", ElementCastArg("SAManagement"), sam)
			.complete() < 0)
		return -1;

	algo = "AES-128/CBC/CTS";
	iv = Botan::InitializationVector("00000000000000000000000000000000");

	return 0;
}

void Crypto::random(uint8_t* buf, unsigned int length) const {
	randombytes_buf(buf, length);
}

/**
 * Return the symmetric shared key for a destination.
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
Hash Crypto::encrypt(const Hash& plain, const SymmetricKey& key) const {
	Botan::Pipe encryptor(get_cipher(algo.c_str(), key, iv, Botan::ENCRYPTION));
	encryptor.process_msg(convert(plain));
	return convert(encryptor.read_all());
}

/**
 * Decrypt cipher using the given key.
 */
Hash Crypto::decrypt(const Hash& cipher, const SymmetricKey& key) const {
	Botan::Pipe decryptor(get_cipher(algo.c_str(), key, iv, Botan::DECRYPTION));
	decryptor.process_msg(convert(cipher));
	return convert(decryptor.read_all());
}

void Crypto::hash(uint8_t* out, const uint8_t* in, unsigned int n) const {
	/** TODO we are doing a truncated version of SHA-256 */
	assert(crypto_hash_sha256_BYTES >= sizeof(Hash));
	uint8_t tmp[crypto_hash_sha256_BYTES];
	crypto_hash_sha256(tmp, in, n);
	memcpy(out, tmp, sizeof(Hash));
}

SValue Crypto::convert(const Hash& data) const {
	return SValue(data.data(), data.size());
}

Hash Crypto::convert(const SValue& data) const {
	return Hash(data.begin());
}

CLICK_ENDDECLS

ELEMENT_LIBS(-L/usr/local/lib -lsodium -lbotan-1.10)
EXPORT_ELEMENT(Crypto)
