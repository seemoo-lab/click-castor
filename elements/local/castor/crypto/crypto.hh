#ifndef CLICK_CRYPTO_HH
#define CLICK_CRYPTO_HH
#include <click/element.hh>
#include "../node_id.hh"
#include "../hash.hh"
#include "samanagement.hh"
#include <botan/botan.h>
#include <sodium.h>

CLICK_DECLS

typedef Botan::SymmetricKey SymmetricKey;

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
	const SymmetricKey* getSharedKey(NodeId) const;
	Hash encrypt(const Hash&, const SymmetricKey&) const;
	Hash decrypt(const Hash&, const SymmetricKey&) const;

	void random(uint8_t* buf, unsigned int length) const;
	template<unsigned int S>
	inline void random(Buffer<S>& buf) const {
		random(buf.data(), buf.size());
	}

	void hash(uint8_t* out, const uint8_t* in, unsigned int n) const;
	template<unsigned int S>
	inline void hash(Hash& out, const Buffer<S>& in) const {
		hash(out.data(), in.data(), in.size());
	}
private:
	SAManagement* sam;
	uint8_t nonce[crypto_stream_aes128ctr_NONCEBYTES];
};

CLICK_ENDDECLS
#endif
