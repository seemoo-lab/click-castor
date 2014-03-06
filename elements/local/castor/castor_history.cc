#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_history.hh"
#include "castor.hh"
#include "crypto.hh"

CLICK_DECLS

CastorHistory::CastorHistory() {
	_history = Vector<Packet*>();
	_ackhistory = Vector<Packet*>();
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

		Packet* q = p->clone();
		_history.push_back(q);

	// Adding an ack to history
	} else if ( type == CASTOR_TYPE_ACK ){

		Packet* q = p->clone();
		_ackhistory.push_back(q);

	}
	else{
		click_chatter("Error adding packet to history, unknown Packet type");
	}
}

/**
 * Check weather packet has already been forwarded
 */
bool CastorHistory::checkDuplicate(Packet* p) {

	uint8_t type = CastorPacket::getType(p);
	if ( type == CASTOR_TYPE_PKT){
		Castor_PKT header;
		CastorPacket::getCastorPKTHeader(p, &header);

		for(int i=0; i<_history.size(); i++){
			Castor_PKT entry;
			CastorPacket::getCastorPKTHeader(_history.at(i), &entry);

			if(! memcmp(entry.fid, header.fid, CASTOR_HASHLENGTH)){
				if(! memcmp(entry.pid, header.pid, CASTOR_HASHLENGTH)){
					// Found a match, this packet is already in history
						return true;
				}
			}
		}
		return false;

	} else if ( type == CASTOR_TYPE_ACK ){
		Castor_ACK header;
		CastorPacket::getCastorACKHeader(p, &header);

		for(int i=0; i<_ackhistory.size(); i++){
			Castor_ACK entry;
			CastorPacket::getCastorACKHeader(_ackhistory.at(i), &entry);

			if(! memcmp(entry.auth, header.auth, CASTOR_HASHLENGTH)){
				if(_ackhistory.at(i)->dst_ip_anno() == p->dst_ip_anno()){
					return true;
				}
			}
		}
		return false;

	} else {
		click_chatter("Error checking duplicates in history, unknown Packet type");
	}
}

/**
 * Search history for certain PID and return the FlowID
 */
Packet* CastorHistory::getPacketById(PacketId pid){

	for(int i=0; i<_history.size(); i++){
		Packet* p = _history.at(i);
		Castor_PKT header;
		CastorPacket::getCastorPKTHeader(p, &header);

		if(! memcmp(header.pid, pid, CASTOR_HASHLENGTH)){
			return p;
		}
	}
	return false;
}

bool CastorHistory::hasACK(PacketId pid){
	// Search the ACK history for ACK corresponding to the PKT
	for(int i=0; i<_ackhistory.size(); i++){
			Packet* a = _ackhistory.at(i);
			Castor_ACK header;
			CastorPacket::getCastorACKHeader(a, &header);
			Hash auth;
			_crypto->hash(&auth, header.auth, sizeof(Hash));

			if(! memcmp(&auth, pid, CASTOR_HASHLENGTH)){
				return true;
			}
		}
		return false;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorHistory)
