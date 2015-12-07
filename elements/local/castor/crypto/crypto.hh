#ifndef CLICK_CRYPTO_HH
#define CLICK_CRYPTO_HH
#include <click/element.hh>
#include "../node_id.hh"
#include "../hash.hh"
#include "samanagement.hh"
#include <botan/botan.h>

CLICK_DECLS

typedef Botan::SecureVector<Botan::byte> SValue;
typedef Botan::Private_Key PrivateKey;
typedef Botan::Public_Key PublicKey;
typedef Botan::SymmetricKey SymmetricKey;

class Crypto: public Element {
public:
	~Crypto();

	const char *class_name() const { return "Crypto"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	/**
	 * Returns a new SymmetricKey instance or NULL if no corresponding key exists
	 */
	const SymmetricKey* getSharedKey(NodeId) const;
	SValue encrypt(const SValue&, const SymmetricKey&) const;
	SValue decrypt(const SValue&, const SymmetricKey&) const;

	void random(uint8_t* buf, unsigned int length) const;
	template<unsigned int S>
	inline void random(Buffer<S>& buf) const { random(buf.data(), buf.size()); }
	SValue hash(const SValue& data) const;
	Hash hash(const Hash& data) const;
	Hash hashConvert(const SValue& data) const;
	SValue hashConvert(const Hash& data) const;

	SValue convert(const Hash& data) const;
	Hash convert(const SValue& data) const;
private:
	SAManagement* sam;
	String algo;
	Botan::InitializationVector iv;
	Botan::HashFunction* hashFunction;
};

CLICK_ENDDECLS
#endif
