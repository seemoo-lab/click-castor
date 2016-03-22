#ifndef CLICK_CRYPTO_HH
#define CLICK_CRYPTO_HH
#include <click/element.hh>
#include "../node_id.hh"
#include "../hash.hh"
#include "samanagement.hh"
#include <sodium.h>

CLICK_DECLS

typedef Vector<uint8_t> SymmetricKey;

class Crypto: public Element {
public:
	const char *class_name() const { return "Crypto"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);
	int initialize(ErrorHandler *);

	/**
	 * Returns a new SymmetricKey instance or NULL if no corresponding key exists
	 */
	const SymmetricKey* getSharedKey(const NodeId&) const;
	const SymmetricKey* getSharedKey(const NeighborId&) const;

	template<unsigned int S>
	inline void stream_xor(Buffer<S>& cipher, const Buffer<S>& plain, const uint8_t* nonce, const SymmetricKey& key) const {
		assert(key.size() == crypto_stream_KEYBYTES);
		crypto_stream_xor(cipher.data(), plain.data(), plain.size(), nonce, key.data());
	}

	inline void stream(uint8_t* out, unsigned int outlen, const uint8_t* nonce, uint8_t* key) const {
		crypto_stream(out, outlen, nonce, key);
	}

	template<unsigned int S>
	inline void random(Buffer<S>& buf) const {
		random(buf.data(), buf.size());
	}

	template<unsigned int S>
	inline void hash(Buffer<S>& out, const uint8_t* in, unsigned int n) const {
		hash(out.data(), out.size(), in, n);
	}
	template<unsigned int S, unsigned int S2>
	inline void hash(Buffer<S>& out, const Buffer<S2>& in) const {
		hash(out.data(), out.size(), in.data(), in.size());
	}

	/**
	 * Expects keys of length crypto_shorthash_KEYBYTES.
	 */
	void auth(Buffer<crypto_shorthash_BYTES>& out,
              const uint8_t* in, unsigned int inlen,
              const uint8_t* key);

	static const unsigned int nonce_size = crypto_stream_NONCEBYTES;
private:
	SAManagement* sam;

	void random(uint8_t* buf, unsigned int length) const;
	void hash(uint8_t* out, unsigned int outlen, const uint8_t* in, unsigned int n) const;
};

CLICK_ENDDECLS
#endif
