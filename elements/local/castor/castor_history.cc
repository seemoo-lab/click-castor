#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_history.hh"

CLICK_DECLS

CastorHistory::CastorHistory() {
	_pkthistory = HashTable<Key, HistoryEntry>();
}

CastorHistory::~CastorHistory() {
}

int CastorHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &_crypto,
		cpEnd);
}

void CastorHistory::addToHistory(Packet* p) {

	uint8_t type = CastorPacket::getType(p);
	// Adding a packet to history
	if ( type == CastorType::PKT){

		addPKTToHistory(p);

	// Adding an ack to history
	} else if ( type == CastorType::ACK ){

		addACKToHistory(p);

	}
	else{
		click_chatter("Error adding packet to history, unknown Packet type");
	}
}

void CastorHistory::addPKTToHistory(Packet* p) {

	Castor_PKT* header;
	header = (Castor_PKT*) p->data();

	HistoryEntry entry;
	entry.routedTo = p->dst_ip_anno();
	entry.ACKedBy = Vector<IPAddress>();
	memcpy(&entry.flow, &header->fid, sizeof(FlowId));

	Key pid;
	memcpy(&pid, header->pid, sizeof(Key));
	_pkthistory.set(pid, entry);
}

void CastorHistory::addACKToHistory(Packet* p) {

	Castor_ACK* header;
	header = (Castor_ACK*) p->data();

	// Determine the Source of the Packet
	IPAddress src = p->dst_ip_anno();

	// Get the entry
	HistoryEntry* entry = getEntryForAuth(header->auth);

	if(!entry) {
		// Received an ACK for an unknown Packet, do not care
		return;
	}

	entry->ACKedBy.push_back(src);
}

bool CastorHistory::ValidateACK(Packet* p){
	Castor_ACK* header;
	header = (Castor_ACK*) p->data();

	// Get the entry
	HistoryEntry* entry = getEntryForAuth(header->auth);

	if(!entry){
		return false;
	}
	return true;
}

void CastorHistory::GetFlowId(Packet* p, FlowId* flow){
	uint8_t type = CastorPacket::getType(p);

	if ( type == CastorType::PKT){
		Castor_PKT* header;
		header = (Castor_PKT*) p->data();

		// Get the entry
		HistoryEntry* entry = getEntryForPid(header->pid);

		memcpy(flow, &entry->flow, sizeof(FlowId));

	} else if ( type == CastorType::ACK ){

		Castor_ACK* header;
		header = (Castor_ACK*) p->data();

		// Get the entry
		HistoryEntry* entry = getEntryForAuth(header->auth);

		memcpy(flow, &entry->flow, sizeof(FlowId));
	}
	else{
		click_chatter("Error adding packet to history, unknown Packet type");
	}
}

/*
 * Determine destination of origin packet
 */
IPAddress CastorHistory::PKTroutedto(Packet* ack){
	Castor_ACK* header;
	header = (Castor_ACK*) ack->data();

	// Get the entry
	HistoryEntry* entry = getEntryForAuth(header->auth);

	if(!entry)
		return IPAddress("0.0.0.0");

	return entry->routedTo;
}

bool CastorHistory::IsFirstACK(Packet* p){
	Castor_ACK* header;
	header = (Castor_ACK*) p->data();

	// Get the entry
	HistoryEntry* entry = getEntryForAuth(header->auth);

	if(entry->ACKedBy.size()>0){
		return false;
	}
	return true;
}



/**
 * Check whether packet has already been forwarded
 */
bool CastorHistory::checkDuplicate(Packet* p) {

	uint8_t type = CastorPacket::getType(p);
	if ( type == CastorType::PKT){

		Castor_PKT* header;
		header = (Castor_PKT*) p->data();

		if(getEntryForPid(header->pid)){
			// Found a match, this packet is already in history
			// TODO If a packet with same pid, but different eauth or payload is received -> NO duplicate
			return true;
		}
		return false;

	} else if ( type == CastorType::ACK ){

		Castor_ACK* header;
		header = (Castor_ACK*) p->data();

		// Determine the Source of the Packet
		IPAddress src = p->dst_ip_anno();

		HistoryEntry* entry = getEntryForAuth(header->auth);
		if(entry){
			// Found a matching packet
			for(int i=0; i<entry->ACKedBy.size();i++) {
				if(entry->ACKedBy.at(i) == src){
					return true;
				}
			}
		}
		return false;

	} else {
		click_chatter("Error checking duplicates in history, unknown Packet type");
		return false;
	}
}

bool CastorHistory::hasACK(PacketId pid){

	HistoryEntry* entry = getEntryForPid(pid);
	if(entry){
		if (entry->ACKedBy.size() > 0){
			return true;
		}
		return false;
	}
	return false;

}

Key CastorHistory::getKeyForPacket(ACKAuth aauth) {
	//Compute the Packet ID corresponding to the ACK
	Hash hash;
	_crypto->hash(hash, aauth, sizeof(ACKAuth));
	Key pid;
	memcpy(&pid, hash, sizeof(Key));
	return pid;
}

HistoryEntry* CastorHistory::getEntryForAuth(ACKAuth aauth) {
	return _pkthistory.get_pointer( getKeyForPacket(aauth) );
}

HistoryEntry* CastorHistory::getEntryForPid(PacketId pid){
	Key key;
	memcpy(&key, pid, sizeof(Key));
	return _pkthistory.get_pointer( key );
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorHistory)
