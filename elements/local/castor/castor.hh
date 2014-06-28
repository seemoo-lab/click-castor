#ifndef CASTOR_H
#define CASTOR_H

#include <click/ipaddress.hh>

#define ETHERTYPE_CASTOR		0x0CA0

#define CASTOR_HASHLENGTH		20
#define CASTOR_ENCLENGTH		128
#define CASTOR_FLOWSIZE			4

#define CASTOR_TYPE_PKT				0xC0
#define CASTOR_TYPE_ACK				0xA0
#define CASTOR_TYPE_MERKLE_PKT		0xCA
#define CASTOR_TYPE_MERKLE_ACK		0xAA
#define CASTOR_CONTENT_TYPE_IP		0x0800

CLICK_DECLS

typedef Vector<char> Bytestring;

typedef uint8_t FlowId[CASTOR_HASHLENGTH];
typedef uint8_t PacketId[CASTOR_HASHLENGTH];
typedef struct {
	uint8_t flow[CASTOR_HASHLENGTH];
} FlowAuth;
typedef uint8_t ACKAuth[CASTOR_HASHLENGTH];
typedef uint8_t EACKAuth[CASTOR_ENCLENGTH];

typedef uint8_t CHash[CASTOR_HASHLENGTH];
typedef uint8_t Hash[CASTOR_HASHLENGTH];

// The Packet Header Structure
typedef struct {
	uint8_t 	type;
	uint8_t 	hsize;
	uint8_t 	fsize;
	uint8_t 	ctype;
	uint16_t 	len;
	IPAddress	src;
	IPAddress	dst;
	FlowId	 	fid;
	PacketId 	pid;
	FlowAuth 	fauth[CASTOR_FLOWSIZE];
	EACKAuth 	eauth;
} Castor_PKT;

// The ACK Header Structure
typedef struct {
	uint8_t 	type;
	uint8_t 	hsize;
	uint16_t 	len;
	ACKAuth 	auth;
} Castor_ACK;

/**
 * The Castor Class with utility functions to handle Packet Processing
 */
class CastorPacket{
public:

	static inline uint8_t getType(Packet* p){
		uint8_t type;
		memcpy(&type, p->data(), sizeof(type));
		type = type & 0xF0;
		return type;
	}

	static inline bool getCastorPKTHeader(Packet* p, Castor_PKT* header){
		if(getType(p) == CASTOR_TYPE_PKT){
			// Copy the header from packet
			memcpy(header, p->data(), sizeof(Castor_PKT));
			return true;
		}
		return false;
	}

	static inline bool getCastorACKHeader(Packet* p, Castor_ACK* header){
		if(getType(p) == CASTOR_TYPE_ACK){
			// Copy the header from packet
			memcpy(header, p->data(), sizeof(Castor_ACK));
			return true;
		}
		return false;
	}

	static String hexToString(const unsigned char* hex, uint8_t length) {
		char buffer[2*length];
		for(int i=0; i<length; i++){
			snprintf(buffer + 2*i, 3,"%02x ", hex[i]);
		}
		return String(buffer);
	}

};

CLICK_ENDDECLS
#endif
