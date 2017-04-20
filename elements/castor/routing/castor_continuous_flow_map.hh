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

#ifndef CLICK_CASTOR_CONTINUOUS_FLOW_MAP_HH
#define CLICK_CASTOR_CONTINUOUS_FLOW_MAP_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "../castor.hh"

CLICK_DECLS

class CastorContinuousFlowMap : public Element {
public:
	const char *class_name() const { return "CastorContinuousFlowMap"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }

	void update(const Hash &flow, const NodeId &src, const NodeId &dst);
	const Hash &previous(const NodeId &src, const NodeId &dst) const;

private:
	HashTable<Pair<NodeId, NodeId>, Hash> srcdstmap;
	HashTable<             NodeId , Hash>    dstmap;

	const Hash default_fid;
};

CLICK_ENDDECLS

#endif //CLICK_CASTOR_CONTINUOUS_FLOW_MAP_HH
