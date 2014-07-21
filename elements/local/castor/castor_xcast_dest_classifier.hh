#ifndef CLICK_CASTOR_XCAST_DEST_CLASSIFIER_HH
#define CLICK_CASTOR_XCAST_DEST_CLASSIFIER_HH

#include <click/element.hh>
#include <click/standard/addressinfo.hh>
#include "castor.hh"
#include "castor_xcast.hh"

CLICK_DECLS

/**
 * Checks whether the incoming Packet needs to be delivered to the local host and/or
 *        whether it needs to be forwarded.
 * Outputs unshared copies.
 */
class CastorXcastDestClassifier: public Element {
public:
	CastorXcastDestClassifier();
	~CastorXcastDestClassifier();

	const char *class_name() const { return "CastorXcastDestClassifier"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	IPAddress myAddr;
};

CLICK_ENDDECLS

#endif
