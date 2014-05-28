#ifndef CLICK_CASTORENCRYPTACKAUTHDUMMY_HH
#define CLICK_CASTORENCRYPTACKAUTHDUMMY_HH
#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"

CLICK_DECLS

class CastorEncryptACKAuthDummy : public Element {
	public:
		CastorEncryptACKAuthDummy();
		~CastorEncryptACKAuthDummy();

		const char *class_name() const	{ return "CastorEncryptACKAuthDummy"; }
		const char *port_count() const	{ return "1/1"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

		void push(int, Packet *);

	private:
		Crypto* _crypto;
		IPAddress _myIP;
};

CLICK_ENDDECLS
#endif
