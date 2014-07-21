#ifndef CLICK_CASTOR_XCAST_FORWARDER_CLASSIFIER_HH
#define CLICK_CASTOR_XCAST_FORWARDER_CLASSIFIER_HH

#include <click/element.hh>
#include <click/standard/addressinfo.hh>
#include "castor.hh"
#include "castor_xcast.hh"

CLICK_DECLS

class CastorXcastForwarderClassifier: public Element {
public:
	CastorXcastForwarderClassifier();
	~CastorXcastForwarderClassifier();

	const char *class_name() const { return "CastorXcastForwarderClassifier"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	IPAddress myAddr;
};

CLICK_ENDDECLS

#endif
