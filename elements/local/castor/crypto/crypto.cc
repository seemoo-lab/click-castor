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
	crypto_hash_sha256(out, in, n);
}

void Crypto::truncated_hash(uint8_t* out, unsigned int outlen, const uint8_t* in, unsigned int inlen) const {
	uint8_t tmp[crypto_hash_sha256_BYTES];
	hash(tmp, in, inlen);
	if (outlen > sizeof(tmp)) {
		click_chatter("Warning: wanting more bytes (%u) than we create (%u)", outlen, sizeof(tmp));
		memcpy(out, tmp, sizeof(tmp));
	} else {
		memcpy(out, tmp, outlen);
	}
}

CLICK_ENDDECLS

ELEMENT_LIBS(-lsodium)
EXPORT_ELEMENT(Crypto)
