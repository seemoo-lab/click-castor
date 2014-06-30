#ifndef CLICK_CASTORDSTCLASSIFIER_HH
#define CLICK_CASTORDSTCLASSIFIER_HH

#include <click/element.hh>
#include <click/standard/addressinfo.hh>
#include "castor.hh"

CLICK_DECLS

class CastorDstClassifier: public Element {
public:
	CastorDstClassifier();
	~CastorDstClassifier();

	const char *class_name() const	{ return "CastorDstClassifier"; }
	const char *port_count() const	{ return "1/2"; }
	const char *processing() const	{ return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	IPAddress myAddr;
};

CLICK_ENDDECLS

#endif
