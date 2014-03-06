#ifndef CLICK_CASTORENCRYPTACKAUTH_HH
#define CLICK_CASTORENCRYPTACKAUTH_HH
#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"

CLICK_DECLS

class CastorEncryptACKAuth : public Element {
	public:
		CastorEncryptACKAuth();
		~CastorEncryptACKAuth();

		const char *class_name() const	{ return "CastorEncryptACKAuth"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

		void push(int, Packet *);

		//int encrypt(uint8_t * toSign,int toSignLength,uint8_t * sig, IPAddress destination);

	private:
		//void testcrypt(uint8_t *, int, IPAddress);
		//void testcrypt(Botan::SecureVector<Botan::byte>, IPAddress);

		//Botan::Private_Key* getPrivateKey(IPAddress address);
		//Botan::Public_Key* getPublicKey(IPAddress address);

		//SAManagement * _sam;
		Crypto* _crypto;
		IPAddress _myIP;
};

CLICK_ENDDECLS
#endif
