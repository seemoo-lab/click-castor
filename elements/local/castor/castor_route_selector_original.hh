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
	CastorRouteSelectorOriginal() : broadcastAdjust(8), routingtable(NULL), neighbors(NULL) {}

	const char *class_name() const { return "CastorRouteSelectorOriginal"; }
	int configure(Vector<String>&, ErrorHandler*);

	/**
	 * Select the best next hop for a given flow/subflow
	 */
	NeighborId select(const FlowId& flow, const NodeId& subflow, const Vector<NodeId>* others, const PacketId &pid);

protected:
	/**
	 * Bandwidth investment for route discovery (larger values reduce the broadcast probability)
	 */
	double broadcastAdjust;

	CastorRoutingTable* routingtable;
	Neighbors* neighbors;

	virtual bool selectNeighbor(const NeighborId &entry, double entryEstimate, Vector<NeighborId> &bestEntries, double &bestEstimate, const PacketId &pid);
	virtual NeighborId chooseNeighbor(Vector<NeighborId> &bestNeighbors, double best, const PacketId &pid);

	double findBest(HashTable<NeighborId, CastorEstimator>& entry, Vector<NeighborId>& bestNeighbors, const PacketId& pid);
	inline NeighborId selectDefault() const { return NeighborId::make_broadcast(); }
};

CLICK_ENDDECLS

#endif
