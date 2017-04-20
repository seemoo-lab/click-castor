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

#ifndef CLICK_SA_MANAGEMENT_HH
#define CLICK_SA_MANAGEMENT_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "securityassociation.hh"
#include "../node_id.hh"
#include "../neighbor_id.hh"

CLICK_DECLS

class SAManagement: public Element {
public:
	const char *class_name() const	{ return "SAManagement"; }
	const char *port_count() const	{ return PORTS_0_0; }
	const char *processing() const	{ return AGNOSTIC; }

	int configure(Vector<String>&, ErrorHandler*);

	const SecurityAssociation* get(const NodeId& node, SecurityAssociation::Type type);
	const SecurityAssociation* get(const NeighborId& node, SecurityAssociation::Type type);
private:
	typedef Vector<SecurityAssociation> SAs;
	HashTable<NodeId, SAs>     sas_e2e; // SAs between end nodes
	HashTable<NeighborId, SAs> sas_h2h; // SAs between neighbors

	size_t symmetricKeyLength;

	void add(const NodeId& node, const SecurityAssociation& sa);
	void add(const NeighborId& node, const SecurityAssociation& sa);

	/**
	 * XXX Warning: insecure key generation
	 * These functions should be only used for testing. It generates unique but deterministic
	 * keys for arbitrary node pairs.
	 * Eventually, keys should be generated using some key exchange protocol, e.g.,
	 *  - pre-deployed keys
	 *  - something similar to Serval DNA (?)
	 *  - Diffie-Hellman
	 *  - ...
	 */
	SecurityAssociation genereateSymmetricSA(const NodeId&);
	SecurityAssociation genereateSymmetricSA(const NeighborId&);
	// Local node's addresses, used to generate shared keys
	NodeId myNodeId;
	NeighborId myNeighborId;
};

CLICK_ENDDECLS

#endif
