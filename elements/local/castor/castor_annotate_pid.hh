#ifndef CLICK_CASTOR_ANNOTATE_PID_HH
#define CLICK_CASTOR_ANNOTATE_PID_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"

CLICK_DECLS

class CastorAnnotatePid: public Element {
public:
	CastorAnnotatePid();
	~CastorAnnotatePid();

	const char *class_name() const { return "CastorAnnotatePid"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	Crypto* crypto;
};

CLICK_ENDDECLS

#endif
