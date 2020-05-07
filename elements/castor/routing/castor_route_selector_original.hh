/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CLICK_CASTOR_ROUTE_SELECTOR_ORIGINAL_HH
#define CLICK_CASTOR_ROUTE_SELECTOR_ORIGINAL_HH

#include <click/element.hh>
#include "castor_route_selector.hh"
#include "../neighbors/neighbors.hh"
#include "castor_routing_table.hh"
#include "castor_continuous_flow_map.hh"
#include "castor_timeout_table.hh"

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
	/**
	 * Break ties based on RTT rather than randomly
	 */
	bool tiebreakerRtt;
	/**
	 * Always unicast starting from here
	 */
	double unicastThreshold;
	/**
	 * Use raw 'reliability' for broadcast decision instead of calculating
	 * e^(-1 * broadcastAdjust * reliability)
	 */
	bool rawReliabilityForBroadcast;

	CastorRoutingTable* routingtable;
	Neighbors* neighbors;
	CastorContinuousFlowMap* continuous_flow;
	CastorTimeoutTable* timeouttable;

	void copy_flow_entry(const Hash &flow, const NodeId &src, const NodeId &dst);
	virtual double select(CastorRoutingTable::FlowEntry& entry, Vector<NeighborId>& best_candidates);
	bool should_unicast(double best_estimate) const;
	const NeighborId& select_random(const Vector<NeighborId>&) const;
	const NeighborId& select_smallest_rtt(const FlowId&, const Vector<NeighborId>&) const;
	inline NeighborId broadcast() const { return NeighborId::make_broadcast(); }
};

CLICK_ENDDECLS

#endif
