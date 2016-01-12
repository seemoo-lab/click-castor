#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "crypto.hh"

CLICK_DECLS

int Crypto::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("SAM", ElementCastArg("SAManagement"), sam)
			.complete();
}

int Crypto::initialize(ErrorHandler* errh) {
	if (sodium_init() == -1) {
		errh->fatal("libsodium could not be initialized");
		return -1;
	} else {
		return 0;
	}
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
void Crypto::auth(Buffer<crypto_onetimeauth_BYTES>& out, const uint8_t* in, unsigned int inlen, const uint8_t* nonce, const uint8_t* key) {
	Buffer<crypto_onetimeauth_KEYBYTES> onetimekey;
	crypto_stream(onetimekey.data(), sizeof(onetimekey), nonce, key);
	crypto_onetimeauth(out.data(), in, inlen, onetimekey.data());
}

int Crypto::auth_verify(const Buffer<crypto_onetimeauth_BYTES>& out, const uint8_t* in, unsigned int inlen, const uint8_t* nonce, const uint8_t* key) {
	Buffer<crypto_onetimeauth_KEYBYTES> onetimekey;
	crypto_stream(onetimekey.data(), sizeof(onetimekey), nonce, key);
	return crypto_onetimeauth_verify(out.data(), in, inlen, onetimekey.data());
}

CLICK_ENDDECLS

ELEMENT_LIBS(-lsodium)
EXPORT_ELEMENT(Crypto)
