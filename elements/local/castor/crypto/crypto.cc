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
	return &sharedKeySA->secret;
}

void Crypto::hash(uint8_t* out, const uint8_t* in, unsigned int n) const {
	/** TODO we are doing a truncated version of SHA-256 */
	static_assert(crypto_hash_sha256_BYTES >= sizeof(Hash), "Hash type must be no larger than 32 bytes");
	uint8_t tmp[crypto_hash_sha256_BYTES];
	crypto_hash_sha256(tmp, in, n);
	memcpy(out, tmp, sizeof(Hash));
}

CLICK_ENDDECLS

ELEMENT_LIBS(-L/usr/local/lib -lsodium)
EXPORT_ELEMENT(Crypto)
