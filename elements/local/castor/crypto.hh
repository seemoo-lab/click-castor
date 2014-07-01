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

	// Public key crypto
	PrivateKey* getPrivateKey(IPAddress);
	PublicKey* getPublicKey(IPAddress);
	SValue encrypt(SValue*, PublicKey*);
	SValue decrypt(SValue*, PrivateKey*);

	// Symmetric crypto
	SymmetricKey* getSharedKey(const IPAddress&);
	SValue encrypt(const SValue&, const SymmetricKey&);
	SValue decrypt(const SValue&, const SymmetricKey&);

	void hash(Hash hash, uint8_t* data, uint8_t length);
	void randomize(Hash r);
	SValue random(int bytes);
	SValue hash(SValue& data);

	void testcrypt(SValue*, IPAddress);
	void testSymmetricCrypt(SValue, IPAddress);

private:
	inline size_t numberOfBlocks(size_t blocksize, size_t ciphersize) {
		return (ciphersize + blocksize - 1) / blocksize; // Round up
	}

	SAManagement* _sam;
	Botan::BlockCipher* blockCipher;

};

CLICK_ENDDECLS
#endif
