/*
 * castor_translate_localhost.hh
 *
 *  Created on: Jun 30, 2014
 *      Author: milan
 */

#ifndef CASTOR_TRANSLATE_LOCALHOST_HH_
#define CASTOR_TRANSLATE_LOCALHOST_HH_

#include <click/element.hh>
#include "castor.hh"

CLICK_DECLS

/**
 * Sets the source IP address of the packet to the specified address
 */
class CastorTranslateLocalhost: public Element {
public:
	CastorTranslateLocalhost();
	~CastorTranslateLocalhost();

	const char *class_name() const { return "CastorTranslateLocalhost"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	IPAddress myAddr;
};

CLICK_ENDDECLS

#endif /* CASTOR_TRANSLATE_LOCALHOST_HH_ */
