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

void Crypto::hash(uint8_t* out, unsigned int outlen, const uint8_t* in, unsigned int inlen) const {
	if (outlen < crypto_generichash_BYTES_MIN || outlen > crypto_generichash_BYTES_MAX) {
		click_chatter("Cannot produce hash values of size %u", outlen);
		return;
	}
	crypto_generichash(out, outlen, in, inlen, NULL, 0);
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
