#ifndef CLICK_CRYPTO_HH
#define CLICK_CRYPTO_HH
#include <click/element.hh>
#include "../protswitch/samanagement.hh"
#include <botan/botan.h>
#include "castor.hh"

CLICK_DECLS

//typedef Botan::SecureVector<Botan::byte> SHash; //DEPRICATED
typedef Botan::SecureVector<Botan::byte> SValue;
typedef Botan::Private_Key 	Private_Key;
typedef Botan::Public_Key 	Public_Key;

class Crypto : public Element {
	public:
		Crypto();
		~Crypto();

		const char *class_name() const	{ return "Crypto"; }
		const char *port_count() const	{ return "0/0"; }
		const char *processing() const	{ return AGNOSTIC; }
		int configure(Vector<String>&, ErrorHandler*);

		Private_Key* 	getPrivateKey	(IPAddress address);
		Public_Key* 	getPublicKey	(IPAddress address);

		SValue 	encrypt	(SValue*, Public_Key*);
		SValue 	decrypt	(SValue*, Private_Key*);

		void hash(Hash* hash, uint8_t* data, uint8_t length);
		void randomize(Hash* r);
		SValue random(int bytes);
		SValue hash(SValue data);

		void testcrypt(SValue*, IPAddress);

	private:
		SAManagement * _sam;
};

CLICK_ENDDECLS
#endif
