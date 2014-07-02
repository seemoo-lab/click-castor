#ifndef CLICK_CASTOR_H
#define CLICK_CASTOR_H

#include <click/ipaddress.hh>

#define ETHERTYPE_CASTOR		0x0CA0
#define CASTOR_CONTENT_TYPE_IP	0x0800

#define CASTOR_HASHLENGTH		20
#define CASTOR_ENCLENGTH		32	// needs to be larger than CASTOR_HASHLENGTH, and multiple of cipher block size (16)#define CASTOR_FLOWSIZE			4	// Number of flow auth elements in the header#define CASTOR_MAX_GROUP_SIZE	10	// Maximal allowed group size

CLICK_DECLS

struct CastorType { // C++11's strongly typed 'enum class' does not work, so create artificial namespace
	enum {
		PKT = 0xC0,
		ACK = 0xA0,

		DUMMY = 0x00,
		DUMMY_PKT = PKT | DUMMY,
		DUMMY_ACK = ACK | DUMMY,

		MERKLE = 0x0A,
		MERKLE_PKT = PKT | MERKLE,
		MERKLE_ACK = ACK | MERKLE,

		XCAST = 0x0C,
		XCAST_PKT = PKT | XCAST,
		XCAST_ACK = ACK | XCAST
	};
};

typedef uint8_t Hash[CASTOR_HASHLENGTH];

typedef Hash FlowId;
typedef Hash PacketId;
typedef struct {
	Hash data;
} FlowAuthElement;
typedef FlowAuthElement FlowAuth[CASTOR_FLOWSIZE];
typedef Hash ACKAuth;
typedef uint8_t EACKAuth[CASTOR_ENCLENGTH];

// The Packet Header Structure
typedef struct {
	uint8_t 	type;
	uint8_t 	hsize;
	uint8_t 	fsize;
	uint8_t 	ctype;
	uint16_t 	len;
	uint16_t	packet_num; // the k-th packet of the current flow, necessary for flow validation (determines whether fauth[i] is left or right sibling in the Merkle tree)
	IPAddress	src;
	IPAddress	dst;
	FlowId	 	fid;
	PacketId 	pid;
	FlowAuth 	fauth;
	EACKAuth 	eauth;
} Castor_PKT;

// The ACK Header Structure
typedef struct {
	uint8_t 	type;
	uint8_t 	hsize;
	uint16_t 	len;
	ACKAuth 	auth;
} Castor_ACK;

// The Packet Header Structure for Explicit Multicast (Xcast)
typedef struct {
	uint8_t 	type;
	uint8_t 	hsize;
	uint8_t 	fsize;
	uint8_t 	ctype;
	uint16_t 	len;
	uint8_t		n; // number of next hops
	IPAddress	src;
	FlowId	 	fid;
	PacketId 	pid;
	FlowAuth 	fauth;
	ACKAuth 	auth; // In contrast to unicast Castor, we include the auth in plaintext
	IPAddress*	rcvs; // intended receivers of the message; size n
	uint8_t*	assign; // size n
	IPAddress*	dsts;
	PacketId*	ipid; // Individual packet ids; sizeof(dests)
} Castor_Xcast_PKT;

// The ACK Header Structure for Explicit Multicast (Xcast)
typedef struct {
	uint8_t  	type;
	uint8_t 	hsize;
	uint16_t 	len;
	EACKAuth 	eauth;
} Castor_Xcast_ACK;

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
		if(getType(p) == CastorType::PKT){
			// Copy the header from packet
			memcpy(header, p->data(), sizeof(Castor_PKT));
			return true;
		}
		return false;
	}

	static inline bool getCastorACKHeader(Packet* p, Castor_ACK* header){
		if(getType(p) == CastorType::ACK){
			// Copy the header from packet
			memcpy(header, p->data(), sizeof(Castor_ACK));
			return true;
		}
		return false;
	}

	static inline bool isXcast(Packet* p) {
		uint8_t type;
		memcpy(&type, p->data(), sizeof(type));
		type = type & 0x0F;
		return (type == CastorType::XCAST);
	}

//	static inline bool isDestinedFor(Packet* p, IPAddress localAddr) {
//
//	}

//	static inline bool getCastorXcastPKTHeader(Packet* p, Castor_Xcast_PKT* header) {
//		if(getType(p) == CASTOR_TYPE_PKT && isXcast(p)) {
//			// Copy fix part
//			memcpy(header, p->data(), sizeof(Castor_Xcast_PKT_Fix));
//
//			char* pt = (char* ) p;
//
//			// Copy next hops
//		 	pt += sizeof(Castor_Xcast_PKT_Fix);
//			header->rcvs = new IPAddress[header->n];
//			memcpy(header->rcvs, pt, header->n * sizeof(IPAddress));
//
//			// Copy assignments
//			pt += header->n * sizeof(IPAddress);
//			header->assign = new uint8_t[header->n];
//			memcpy(header->assign, pt, header->n * sizeof(uint8_t));
//
//			// Copy destinations
//			pt += header->n * sizeof(uint8_t));
//			unsigned int ndsts = 0;
//			for(int i = 0; i < header->n; i++)
//				ndsts += header->assign[i]; // Count number of Xcast destinations
//			header->dsts = new IPAddress[ndsts];
//			memcpy(header->assign, pt, ndsts * sizeof(IPAddress));
//
//			// Copy individual pids
//			pt += ndsts * sizeof(IPAddress);
//			header->ipid = new PacketId[ndsts];
//
//
//
//			return true;
//		}
//		return false;
//	};
//
//	static inline bool getCastorXcastACKHeader(Packet* p, Castor_Xcast_ACK* header) {
//		if(getType(p) == CASTOR_TYPE_ACK && isXcast(p)) {
//			memcpy(header, p->data(), sizeof(Castor_Xcast_ACK));
//			return true;
//		}
//		return false;
//	};

	static inline String hexToString(const unsigned char* hex, uint8_t length) {
		char buffer[2*length];
		for(int i=0; i<length; i++){
			snprintf(buffer + 2*i, 3,"%02x ", hex[i]);
		}
		return String(buffer);
	}

};

CLICK_ENDDECLS
#endif
