#ifndef CLICK_CASTOR_ROUTE_SELECTOR_ORIGINAL_HH
#define CLICK_CASTOR_ROUTE_SELECTOR_ORIGINAL_HH

#include <click/element.hh>
#include "castor_route_selector.hh"
#include "../neighbordiscovery/neighbors.hh"
#include "castor_routing_table.hh"

CLICK_DECLS

/**
 * Chooses next hop as described in the 2010 Castor paper
 */
class CastorRouteSelectorOriginal: public CastorRouteSelector {
public:
	const char *class_name() const { return "CastorRouteSelectorOriginal"; }
	int configure(Vector<String>&, ErrorHandler*);

	/**
	 * Select the best next hop for a given flow
	 */
	NeighborId select(const Hash& flow, const NodeId& src, const NodeId& dst);
protected:
	/**
	 * Bandwidth investment for route discovery (larger values reduce the broadcast probability)
	 */
	double broadcastAdjust;

	CastorRoutingTable* routingtable;
	Neighbors* neighbors;

	virtual double select(HashTable<NeighborId, CastorEstimator>& entry, Vector<NeighborId>& best_candidates);
	virtual void update_candidates(const NeighborId&, double, Vector<NeighborId>&, double&) const;
	bool should_broadcast(double best_estimate) const;
	const NeighborId& select_random(const Vector<NeighborId>&) const;
	inline NeighborId broadcast() const { return NeighborId::make_broadcast(); }
};

CLICK_ENDDECLS

#endif
