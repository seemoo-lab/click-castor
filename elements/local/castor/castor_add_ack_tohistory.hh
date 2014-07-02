#ifndef CLICK_CASTOR_ADD_ACK_TOHISTORY_HH
#define CLICK_CASTOR_ADD_ACK_TOHISTORY_HH
#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"
#include "castor_history.hh"

CLICK_DECLS

class CastorAddACKToHistory: public Element {
public:
	CastorAddACKToHistory();
	~CastorAddACKToHistory();

	const char *class_name() const	{ return "CastorAddACKToHistory"; }
	const char *port_count() const	{ return PORTS_1_1; }
	const char *processing() const	{ return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	Crypto* crypto;
	CastorHistory* history;
};

CLICK_ENDDECLS
#endif
