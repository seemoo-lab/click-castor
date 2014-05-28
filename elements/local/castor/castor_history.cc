#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_history.hh"
#include "castor.hh"
#include "crypto.hh"

CLICK_DECLS

CastorHistory::CastorHistory() {
	_pkthistory = HashTable<String, HistoryEntry>();
}

CastorHistory::~CastorHistory() {
}

int CastorHistory::configure(Vector<String> &conf, ErrorHandler *errh)
{
	int res = cp_va_kparse(conf, this, errh,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &_crypto,
		cpEnd);
	if(res < 0) return res;
	return 0;
}

void CastorHistory::addToHistory(Packet* p) {

	uint8_t type = CastorPacket::getType(p);
	// Adding a packet to history
	if ( type == CASTOR_TYPE_PKT){

		addPKTToHistory(p);

	// Adding an ack to history
	} else if ( type == CASTOR_TYPE_ACK ){

		addACKToHistory(p);

	}
	else{
		click_chatter("Error adding packet to history, unknown Packet type");
	}
}

void CastorHistory::addPKTToHistory(Packet* p) {

	Castor_PKT* header;
	header = (Castor_PKT*) p->data();

	// Cast Packet.ID
	String pid = String(header->pid);
	HistoryEntry entry;
	entry.routedTo = p->dst_ip_anno();
	entry.ACKedBy = Vector<IPAddress>();
	memcpy(&entry.flow, &header->fid, sizeof(FlowId));

	_pkthistory.set(pid,entry);
}

void CastorHistory::addACKToHistory(Packet* p) {

	Castor_ACK* header;
	header = (Castor_ACK*) p->data();

	// Determine the Source of the Packet
	IPAddress src = p->dst_ip_anno();

	//Compute the Packet ID corresponding to the ACK
	Hash pid;
	_crypto->hash(&pid, header->auth, sizeof(Hash));

	// Get the entry
	HistoryEntry* entry = _pkthistory.get_pointer(String(pid));

	if(!entry){
		// Received an ACK for an unknown Packet, do not care
		return;
	}

	entry->ACKedBy.push_back(src);
}

bool CastorHistory::ValidateACK(Packet* p){
	Castor_ACK* header;
	header = (Castor_ACK*) p->data();

	// Determine the Source of the Packet
	IPAddress src = p->dst_ip_anno();

	//Compute the Packet ID corresponding to the ACK
	Hash pid;
	_crypto->hash(&pid, header->auth, sizeof(Hash));

	// Get the
	HistoryEntry* entry = _pkthistory.get_pointer(String(pid));

	if(!entry){
		return false;
	}
	return true;
}

void CastorHistory::GetFlowId(Packet* p, FlowId* flow){
	uint8_t type = CastorPacket::getType(p);

	if ( type == CASTOR_TYPE_PKT){
		Castor_PKT* header;
		header = (Castor_PKT*) p->data();

		// Cast Packet.ID
		String pid = String(header->pid);

		memcpy(flow, &_pkthistory.get_pointer(pid)->flow, sizeof(FlowId));

	} else if ( type == CASTOR_TYPE_ACK ){

		Castor_ACK* header;
		header = (Castor_ACK*) p->data();

		//Compute the Packet ID corresponding to the ACK
		Hash pid;
		_crypto->hash(&pid, header->auth, sizeof(Hash));

		// Get the
		HistoryEntry* entry = _pkthistory.get_pointer(String(pid));

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

	// Determine the Source of the Packet
	IPAddress src = ack->dst_ip_anno();

	//Compute the Packet ID corresponding to the ACK
	Hash pid;
	_crypto->hash(&pid, header->auth, sizeof(Hash));

	// Get the
	HistoryEntry* entry = _pkthistory.get_pointer(String(pid));

	if(!entry)
		return IPAddress("0.0.0.0");

	return entry->routedTo;
}

bool CastorHistory::IsFirstACK(Packet* p){
	Castor_ACK* header;
	header = (Castor_ACK*) p->data();

	// Determine the Source of the Packet
	IPAddress src = p->dst_ip_anno();

	//Compute the Packet ID corresponding to the ACK
	Hash pid;
	_crypto->hash(&pid, header->auth, sizeof(Hash));

	// Get the
	HistoryEntry* entry = _pkthistory.get_pointer(String(pid));

	if(entry->ACKedBy.size()>0){
		return false;
	}
	return true;
}



/**
 * Check weather packet has already been forwarded
 */
bool CastorHistory::checkDuplicate(Packet* p) {

	uint8_t type = CastorPacket::getType(p);
	if ( type == CASTOR_TYPE_PKT){

		Castor_PKT* header;
		header = (Castor_PKT*) p->data();

		// Cast Packet.ID
		String pid = String(header->pid);

		if(_pkthistory.get_pointer(pid)){
			// Found a match, this packet is already in history
			return true;
		}
		return false;

	} else if ( type == CASTOR_TYPE_ACK ){

		Castor_ACK* header;
		header = (Castor_ACK*) p->data();

		// Determine the Source of the Packet
		IPAddress src = p->dst_ip_anno();

		//Compute the Packet ID corresponding to the ACK
		Hash pid;
		_crypto->hash(&pid, header->auth, sizeof(Hash));

		HistoryEntry* entry = _pkthistory.get_pointer(String(pid));
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

	HistoryEntry* entry = _pkthistory.get_pointer(String(pid));
	if(entry){
		if (entry->ACKedBy.size() > 0){
			return true;
		}
		return false;
	}
	click_chatter("Could not found corresponding packet in history");
	return false;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorHistory)
