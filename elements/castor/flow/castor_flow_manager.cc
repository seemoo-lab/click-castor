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

#include <click/config.h>
#include <click/args.hh>
#include "castor_flow_manager.hh"
#include "castor_merkle_flow.hh"

CLICK_DECLS

CastorFlowManager::CastorFlowManager() : _flows(HashTable<NodeId, CastorMerkleFlow>(CastorMerkleFlow()))
{
}

int CastorFlowManager::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("SIZE", _flowsize)
			.read_mp("FlowTable", ElementCastArg("CastorFlowTable"), _flowtable)
			.read_mp("Crypto", ElementCastArg("Crypto"), _crypto)
			.complete();
}

const FlowId &CastorFlowManager::getCurrentFlowId(NodeId src, NodeId dst) {
	CastorMerkleFlow &flow = createFlowIfNotExists(src, dst);

	return flow.getFlowId();
}

PacketLabel CastorFlowManager::getPacketLabel(NodeId src, NodeId dst) {
	assert(!src.empty() && !dst.empty() && !src.is_multicast());

	CastorMerkleFlow &flow = createFlowIfNotExists(src, dst);

	return flow.freshLabel();
}

CastorMerkleFlow &CastorFlowManager::createFlowIfNotExists(NodeId src, NodeId dst) {
	bool has_flow = _flows.count(src) > 0 && _flows[src].count(dst) > 0;

	if (!has_flow || !_flows[src][dst].isAlive()) {
		_flows[src].set(dst, CastorMerkleFlow(_flowsize, dst, _flowtable, _crypto));
	}

	return _flows[src][dst];
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorFlowManager)
