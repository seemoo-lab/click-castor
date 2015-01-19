#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_history.hh"

CLICK_DECLS

CastorHistory::CastorHistory() {
	history = HashTable<Hash, CastorHistoryEntry>();
}

void CastorHistory::addPkt(const PacketId& pid, const FlowId& fid, IPAddress prevHop, IPAddress nextHop, IPAddress destination) {
	CastorHistoryEntry* entry = getEntry(pid);
	assert(prevHop.addr() != 0);
	if(!entry) {
		CastorHistoryEntry entry;
		entry.destination = destination;
		entry.prevHops.push_back(prevHop);
		entry.nextHop = nextHop;
		entry.expired = false;
		entry.recievedACKs = Vector<IPAddress>();
		entry.fid = fid;
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

bool CastorHistory::addFirstAckForCastor(const PacketId& pid, IPAddress addr, const ACKAuth& ackAuth) {
	CastorHistoryEntry* entry = getEntry(pid);
	if(!entry) {
		// Received an ACK for an unknown Packet, do not care
		click_chatter("Error: trying to add ACK for unknown PKT");
		return false;
	}
	entry->recievedACKs.push_back(addr);
	memcpy(&entry->auth, &ackAuth, sizeof(ACKAuth));
	return true;
}

bool CastorHistory::addFirstAckForXcastor(const PacketId& pid, IPAddress addr, const EACKAuth& ackAuth) {
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

bool CastorHistory::addAckFor(const PacketId& pid, IPAddress addr) {
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

bool CastorHistory::hasPktFrom(const PacketId& pid, IPAddress addr) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	if (entry) {
		for (int i = 0; i < entry->prevHops.size(); i++)
			if (entry->prevHops[i] == addr)
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

bool CastorHistory::hasAckFrom(const PacketId& pid, IPAddress addr) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	if (entry) {
		for (int i = 0; i < entry->recievedACKs.size(); i++)
			if (entry->recievedACKs[i] == addr)
				return true;
	}
	return false;
}

size_t CastorHistory::getPkts(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->prevHops.size();
}
const Vector<IPAddress>& CastorHistory::getPktSenders(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->prevHops;
}

size_t CastorHistory::getAcks(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->recievedACKs.size();
}

const FlowId& CastorHistory::getFlowId(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->fid;
}

const EACKAuth& CastorHistory::getEAckAuth(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->auth;
}

const ACKAuth& CastorHistory::getAckAuth(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->auth;
}

IPAddress CastorHistory::getDestination(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->destination;
}

IPAddress CastorHistory::routedTo(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->nextHop;
}

void CastorHistory::setExpired(const PacketId& pid) {
	CastorHistoryEntry* entry = getEntry(pid);
	entry->expired = true;
}

bool CastorHistory::isExpired(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry && entry->expired;
}

const CastorHistory::CastorHistoryEntry* CastorHistory::getEntry(const PacketId& pid) const {
	return history.get_pointer(pid);
}

CastorHistory::CastorHistoryEntry* CastorHistory::getEntry(const PacketId& pid) {
	return history.get_pointer(pid);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorHistory)
