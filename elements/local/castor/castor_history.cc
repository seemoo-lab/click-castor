#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_history.hh"

CLICK_DECLS

CastorHistory::CastorHistory() {
	history = HashTable<Key, CastorHistoryEntry>();
}

CastorHistory::~CastorHistory() {
}

void CastorHistory::addPkt(const PacketId& pid, const FlowId& fid, IPAddress nextHop, IPAddress destination) {
	CastorHistoryEntry entry;
	entry.destination = destination;
	entry.nextHop = nextHop;
	entry.expired = false;
	entry.recievedACKs = Vector<IPAddress>();
	memcpy(&entry.fid, fid, sizeof(FlowId));

	history.set(pidToKey(pid), entry);
}

bool CastorHistory::addAckFor(const PacketId& pid, IPAddress addr) {
	CastorHistoryEntry* entry = getEntry(pid);
	if(!entry) {
		// Received an ACK for an unknown Packet, do not care
		click_chatter("Error: trying to add ACK for unknown PKT");
		return false;
	}
	entry->recievedACKs.push_back(addr);
	return true;
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

size_t CastorHistory::getAcks(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->recievedACKs.size();
}

const FlowId& CastorHistory::getFlowId(const PacketId& pid) const {
	const CastorHistoryEntry* entry = getEntry(pid);
	return entry->fid;
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

CastorHistory::Key CastorHistory::pidToKey(const PacketId& pid) const {
	Key key;
	memcpy(&key, pid, sizeof(Key));
	return key;
}

const CastorHistory::CastorHistoryEntry* CastorHistory::getEntry(const PacketId& pid) const {
	return history.get_pointer(pidToKey(pid));
}

CastorHistory::CastorHistoryEntry* CastorHistory::getEntry(const PacketId& pid) {
	return history.get_pointer(pidToKey(pid));
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorHistory)
