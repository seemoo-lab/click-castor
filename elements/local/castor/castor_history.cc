#include <click/config.h>
#include "castor_history.hh"

CLICK_DECLS

void CastorHistory::addPkt(const PacketId& pid, const FlowId& fid, const NeighborId& prevHop, const NeighborId& nextHop, const NodeId& destination, Timestamp timestamp) {
	CastorHistoryEntry* entry = getEntry(pid);
	if(!entry) {
		CastorHistoryEntry entry;
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

const FlowId& CastorHistory::getFlowId(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->fid;
}

const AckAuth& CastorHistory::getAckAuth(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->auth;
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
