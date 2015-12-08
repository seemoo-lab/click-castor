#include <click/config.h>
#include <click/args.hh>
#include "crypto.hh"

CLICK_DECLS

int Crypto::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("SAM", ElementCastArg("SAManagement"), sam)
			.complete();
}

int Crypto::initialize(ErrorHandler* errh) {
	// We believe that since we only encrypt pseudorandom nonces,
	// we can use the same nonce for all encryptions.
	memset(nonce, 0, sizeof(nonce));
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
 * Encrypt plain using AES-128 in CTR mode.
 */
Hash Crypto::encrypt(const Hash& plain, const SymmetricKey& key) const {
	assert(key.length() == crypto_stream_aes128ctr_KEYBYTES);
	Hash out;
	crypto_stream_aes128ctr_xor(out.data(), plain.data(), plain.size(), nonce, key.begin());
	return out;
}

/**
 * Decrypt cipher using the given key.
 */
Hash Crypto::decrypt(const Hash& cipher, const SymmetricKey& key) const {
	return encrypt(cipher, key);
}

void Crypto::hash(uint8_t* out, const uint8_t* in, unsigned int n) const {
	/** TODO we are doing a truncated version of SHA-256 */
	assert(crypto_hash_sha256_BYTES >= sizeof(Hash));
	uint8_t tmp[crypto_hash_sha256_BYTES];
	crypto_hash_sha256(tmp, in, n);
	memcpy(out, tmp, sizeof(Hash));
}

CLICK_ENDDECLS

ELEMENT_LIBS(-L/usr/local/lib -lsodium)
EXPORT_ELEMENT(Crypto)
