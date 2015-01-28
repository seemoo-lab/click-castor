#ifndef CLICK_CASTOR_ROUTE_SELECTOR_ORIGINAL_HH
#define CLICK_CASTOR_ROUTE_SELECTOR_ORIGINAL_HH

#include <click/element.hh>
#include "castor_route_selector.hh"
#include "castor_routingtable.hh"
#include "../neighbordiscovery/neighbors.hh"

CLICK_DECLS

/**
 * Chooses next hop as described in the 2010 Castor paper
 */
class CastorRouteSelectorOriginal: public CastorRouteSelector {
public:
	CastorRouteSelectorOriginal();

	const char *class_name() const { return "CastorRouteSelectorOriginal"; }
	int configure(Vector<String>&, ErrorHandler*);

	/**
	 * Select the best next hop for a given flow/subflow
	 */
	IPAddress select(const FlowId& flow, IPAddress subflow, const Vector<IPAddress>* others, const PacketId &pid);

protected:
	virtual void selectNeighbor(const IPAddress &entry, double entryEstimate, Vector<IPAddress> &bestEntries, double &bestEstimate, const PacketId &pid);

	CastorRoutingTable* routingtable;
	Neighbors* neighbors;

	/**
	 * Bandwidth investment for route discovery (larger values reduce the broadcast probability)
	 */
	double broadcastAdjust;

	double findBest(HashTable<IPAddress, CastorEstimator>& entry, Vector<IPAddress>& bestNeighbors, const PacketId& pid);
	IPAddress selectDefault() const;
};

CLICK_ENDDECLS

#endif
