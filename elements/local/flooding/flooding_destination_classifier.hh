#ifndef CLICK_FLOODING_DESTINATION_CLASSIFIER_HH
#define CLICK_FLOODING_DESTINATION_CLASSIFIER_HH

#include <click/element.hh>
#include "flooding.hh"
#include "../castor/castor_xcast_destination_map.hh"

CLICK_DECLS

class FloodingDestinationClassifier : public Element {
	public:
		const char *class_name() const	{ return "FloodingDestinationClassifier"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

		void push(int, Packet *);

	private:
		CastorXcastDestinationMap* map;
		IPAddress myIP;
};

CLICK_ENDDECLS

#endif
