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

#ifndef CLICK_CASTOR_FLOW_MANAGER_HH
#define CLICK_CASTOR_FLOW_MANAGER_HH

#include <click/element.hh>
#include "castor_merkle_flow.hh"
#include "castor_flow_table.hh"
#include "../node_id.hh"
#include "../crypto/crypto.hh"

CLICK_DECLS

class CastorFlowManager : public Element {
public:
	CastorFlowManager();

	const char *class_name() const { return "CastorFlowManager"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }

	int configure(Vector<String>&, ErrorHandler*);

	PacketLabel getPacketLabel(NodeId, NodeId);
	const FlowId &getCurrentFlowId(NodeId, NodeId);
private:
	unsigned int _flowsize;
	CastorFlowTable* _flowtable;
	Crypto* _crypto;
	HashTable<NodeId, HashTable<NodeId, CastorMerkleFlow> > _flows;
	CastorMerkleFlow &createFlowIfNotExists(NodeId src, NodeId dst);
};

CLICK_ENDDECLS

#endif
