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
#include "castor_history.hh"

CLICK_DECLS

void CastorHistory::addPkt(const PacketId& pid, unsigned int k, const FlowId& fid, const NeighborId& prevHop, const NeighborId& nextHop, const NodeId& source, const NodeId& destination, Timestamp timestamp) {
	CastorHistoryEntry* entry = getEntry(pid);
	if(!entry) {
		CastorHistoryEntry entry;
		entry.k = k;
		entry.source = source;
		entry.destination = destination;
		entry.prevHops.push_back(prevHop);
		entry.nextHop = nextHop;
		entry.fid = fid;
		entry.timestamp = timestamp;
		history.set(pid, entry);
	} else {
		// Entry already exists, just add prevHop to list if it does not already exist
		bool alreadyContains = false;
		for (int i = 0; i < entry->prevHops.size(); i++) {
			if (entry->prevHops[i] == prevHop) {
				alreadyContains = true;
				break;
			}
		}
		if(!alreadyContains)
			entry->prevHops.push_back(prevHop);
	}
}

bool CastorHistory::addFirstAckFor(const PacketId& pid, const NeighborId& addr, const AckAuth& ackAuth) {
	CastorHistoryEntry* entry = getEntry(pid);
	if(!entry) {
		// Received an ACK for an unknown Packet, do not care
		click_chatter("Error: trying to add ACK for unknown PKT");
		return false;
	}
	entry->recievedACKs.push_back(addr);
	entry->auth = ackAuth;
	return true;
}

bool CastorHistory::addAckFor(const PacketId& pid, const NeighborId& addr) {
	CastorHistoryEntry* entry = getEntry(pid);
	if(!entry) {
		// Received an ACK for an unknown Packet, do not care
		click_chatter("Error: trying to add ACK for unknown PKT");
		return false;
	}
	assert(entry->recievedACKs.size() > 0);
	entry->recievedACKs.push_back(addr);
	return true;
}

bool CastorHistory::hasPktFrom(const PacketId& pid, const NeighborId& addr) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	if (entry) {
		for (const auto& prevHop : entry->prevHops)
			if (prevHop == addr)
				return true;
	}
	return false;
}

bool CastorHistory::hasPkt(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry;
}

bool CastorHistory::hasAck(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry && !entry->recievedACKs.empty();
}

bool CastorHistory::hasAckFrom(const PacketId& pid, const NeighborId& addr) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	if (entry) {
		for (int i = 0; i < entry->recievedACKs.size(); i++)
			if (entry->recievedACKs[i] == addr)
				return true;
	}
	return false;
}

const Vector<NeighborId>& CastorHistory::getPktSenders(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->prevHops;
}

unsigned int CastorHistory::k(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->k;
}

const FlowId& CastorHistory::getFlowId(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->fid;
}

const AckAuth& CastorHistory::getAckAuth(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->auth;
}

const NodeId& CastorHistory::getSource(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->source;
}

const NodeId& CastorHistory::getDestination(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->destination;
}

const NeighborId& CastorHistory::routedTo(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->nextHop;
}

bool CastorHistory::remove(const PacketId& pid) {
	return history.erase(pid) > 0;
}

const Timestamp& CastorHistory::getTimestamp(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->timestamp;
}

const CastorHistory::CastorHistoryEntry* CastorHistory::getEntry(const PacketId& pid) const {
	return history.get_pointer(pid);
}

CastorHistory::CastorHistoryEntry* CastorHistory::getEntry(const PacketId& pid) {
	return history.get_pointer(pid);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorHistory)
