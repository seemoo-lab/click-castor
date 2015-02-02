#ifndef CLICK_CASTOR_ROUTE_SELECTOR_EXPERIMENTAL_HH
#define CLICK_CASTOR_ROUTE_SELECTOR_EXPERIMENTAL_HH

#include <click/element.hh>
#include "castor_route_selector_original.hh"
#include "castor_history.hh"

CLICK_DECLS

/**
 * Modifies the next-hop selection as described in the 2010 Castor paper
 */
class CastorRouteSelectorExperimental: public CastorRouteSelectorOriginal {
public:
	const char *class_name() const { return "CastorRouteSelectorExperimental"; }
	int configure(Vector<String>&, ErrorHandler*);

private:
	bool selectNeighbor(const NodeId &entry, double entryEstimate, Vector<NodeId> &bestEntries, double &bestEstimate, const PacketId &pid);
	NodeId chooseNeighbor(Vector<NodeId> &bestNeighbors, double best, const PacketId &pid);

	CastorHistory* history;
};

CLICK_ENDDECLS

#endif
