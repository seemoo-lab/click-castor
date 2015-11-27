#ifndef CLICK_CASTOR_XCAST_DEST_CLASSIFIER_HH
#define CLICK_CASTOR_XCAST_DEST_CLASSIFIER_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_xcast.hh"
#include "../neighbordiscovery/neighbor_id.hh"

CLICK_DECLS

/**
 * Checks whether the incoming Packet needs to be delivered to the local host and/or
 *        whether it needs to be forwarded.
 * Outputs unshared copies.
 */
class CastorXcastDestClassifier: public Element {
public:
	const char *class_name() const { return "CastorXcastDestClassifier"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	NeighborId my_id;
	NodeId my_end_node_id;
};

CLICK_ENDDECLS

#endif
