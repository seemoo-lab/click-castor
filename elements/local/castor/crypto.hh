#ifndef CLICK_CRYPTO_HH
#define CLICK_CRYPTO_HH
#include <click/element.hh>
#include "../protswitch/samanagement.hh"
#include <botan/botan.h>
#include "castor.hh"

CLICK_DECLS

typedef Botan::SecureVector<Botan::byte> SValue;
typedef Botan::Private_Key PrivateKey;
typedef Botan::Public_Key PublicKey;
typedef Botan::SymmetricKey SymmetricKey;

class Crypto: public Element {

public:
	Crypto();
	~Crypto();

	const char *class_name() const { return "Crypto"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	/**
	 * Returns a new SymmetricKey instance or NULL if no corresponding key exists
	 */
	const SymmetricKey* getSharedKey(IPAddress) const;
	SValue encrypt(const SValue&, const SymmetricKey&) const;
	SValue decrypt(const SValue&, const SymmetricKey&) const;

	void hash(Hash hash, const uint8_t* data, uint8_t length) const;
	SValue random(int bytes) const;
	SValue hash(const SValue& data) const;

	void testSymmetricCrypt(SValue, IPAddress) const;

private:
	inline size_t numberOfBlocks(size_t blocksize, size_t ciphersize) const {
		return (ciphersize + blocksize - 1) / blocksize; // Round up
	}

	SAManagement* _sam;
	Botan::BlockCipher* blockCipher;

};

CLICK_ENDDECLS
#endif
