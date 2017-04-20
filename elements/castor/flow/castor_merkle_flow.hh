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

#ifndef CLICK_CASTOR_MERKLE_FLOW_HH
#define CLICK_CASTOR_MERKLE_FLOW_HH

#include <click/vector.hh>
#include "../crypto/crypto.hh"
#include "castor_flow_table.hh"
#include "castor_flow_entry.hh"

CLICK_DECLS

class CastorFlowManager;

class PacketLabel {
public:
	PacketLabel(unsigned int num, unsigned int size, const FlowId &fid, const PacketId &pid, const Nonce &n)
			: num(num), size(size), fid(fid), pid(pid), n(n) { }
	unsigned int num;
	unsigned int size;
	const FlowId &fid;
	const PacketId &pid;
	const Nonce &n;
};

class CastorMerkleFlow {
public:
	CastorMerkleFlow(size_t size, const NodeId& dst, CastorFlowTable* flowtable, const Crypto* crypto);
	PacketLabel freshLabel();
	const FlowId &getFlowId() const;
	bool isAlive() const;
private:
	friend class CastorFlowManager;
	// Dummy constructor
	CastorMerkleFlow() : pos(0), flowtable(NULL) { }

	FlowId fid;
	unsigned int pos;

	CastorFlowTable* flowtable;
};

CLICK_ENDDECLS

#endif
