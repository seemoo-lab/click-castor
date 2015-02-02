#ifndef CLICK_CASTOR_TRANSLATE_LOCALHOST_HH
#define CLICK_CASTOR_TRANSLATE_LOCALHOST_HH

#include <click/element.hh>
#include <click/ipaddress.hh>
#include "castor.hh"

CLICK_DECLS

/**
 * Sets the source IP address of the packet to the specified address
 */
class CastorTranslateLocalhost: public Element {
public:
	const char *class_name() const { return "CastorTranslateLocalhost"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	IPAddress myAddr;
};

CLICK_ENDDECLS

#endif
