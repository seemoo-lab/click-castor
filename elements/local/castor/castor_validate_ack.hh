#ifndef CLICK_CASTOR_VALIDATEACK_HH
#define CLICK_CASTOR_VALIDATEACK_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"
#include "castor_history.hh"

CLICK_DECLS

// XXX Should be named CastorAuthenticateAck

class CastorValidateACK: public Element {
public:
	CastorValidateACK();
	~CastorValidateACK();

	const char *class_name() const { return "CastorValidateACK"; }
	const char *port_count() const { return "1/4"; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	Crypto* crypto;
	CastorHistory* history;
};

CLICK_ENDDECLS

#endif
