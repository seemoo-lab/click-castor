#ifndef CLICK_CASTOR_HH
#define CLICK_CASTOR_HH

#include <click/packet.hh>
#include "node_id.hh"
#include "hash.hh"

//#define DEBUG_ACK_SRCDST  // uncomment to add source and destination fields to ACK packets
#define DEBUG_HOPCOUNT // include (unprotected) hopcount field in packets

#define CASTOR_FLOWAUTH_ELEM                         8  // Number of flow auth elements

#define icv_BYTES 8U
#define nonce_BYTES 24U

CLICK_DECLS

namespace CastorType { // C++11's strongly typed 'enum class' does not work, so create artificial namespace
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
	};
};

typedef Hash FlowId;
typedef Hash PacketId;
typedef struct {
	Hash& operator[](int i) { return elem[i]; }
	Hash elem[CASTOR_FLOWAUTH_ELEM];
} FlowAuth;
typedef Hash AckAuth;
typedef Hash PktAuth;
typedef Buffer<icv_BYTES> ICV;
typedef Buffer<nonce_BYTES> Nonce;

/**
 * The Castor data packet header (PKT)
 */
class CastorPkt {
public:
	uint8_t 	type;    // = MERKLE_PKT
	uint8_t 	hsize;   // size of the hash values in this header
	uint16_t 	len;     // total length of the PKT (incl. payload)
	uint8_t		syn : 1; // first PKT(s) of flow
						 // 'syn' is set until the first ACK for the flow is received
	uint8_t		arq : 1; // request retransmission of PKT
#ifdef DEBUG_HOPCOUNT
	uint8_t		hopcount : 6;
#else
	uint8_t		_        : 6; // padding
#endif
	uint8_t 	fsize  : 4; // = Merkle tree height = log2(number of leaves)
	uint8_t 	fasize : 4; // = number of flow authentication elements in [0..fsize]
	uint16_t	kpkt; // the k-th packet of the current flow, necessary for flow validation (determines whether fauth[i] is left or right sibling in the Merkle tree)
	NodeId		src;
	NodeId		dst;
	FlowId	 	fid;
	PacketId 	pid;
	ICV			icv;
	/* included if SYN = 1 */
	// Nonce	    n;
	inline const Nonce* n() const { return reinterpret_cast<const Nonce*>((uint8_t*) this + sizeof(*this)); }
	inline Nonce* n() { return reinterpret_cast<Nonce*>((uint8_t*) this + sizeof(*this)); }
	/* variable size 0..fsize */
	// Hash fauth[fasize];
	inline const Hash* fauth() const { return reinterpret_cast<const Hash*>((uint8_t*) this + sizeof(*this) + ((syn) ? sizeof(Nonce) : 0)); }
	inline Hash* fauth() { return reinterpret_cast<Hash*>((uint8_t*) this + sizeof(*this) + ((syn) ? sizeof(Nonce) : 0)); }

	inline unsigned int header_len() const {
		return sizeof(*this) + (syn ? sizeof(Nonce) : 0) + fasize * hsize;
	}
	inline unsigned int payload_len() const {
		return ntohs(len) - header_len();
	}
};

/**
 * The Castor acknowledgement packet (ACK)
 */
class CastorAck {
public:
	uint8_t 	type;
	uint8_t 	hsize;
	uint16_t 	len;
#ifdef DEBUG_ACK_SRCDST
	NodeId		src;
	NodeId		dst;
#endif
	AckAuth 	auth;
};

/**
 * Utility class to handle packet types and annotations
 */
class CastorPacket {
public:
	static inline uint8_t getType(const Packet* p) {
		uint8_t type = p->data()[0] & 0xF0;
		return type;
	}
	static inline bool isXcast(Packet* p) {
		uint8_t type = p->data()[0] & 0x0F;
		return (type == CastorType::XCAST);
	}
};

CLICK_ENDDECLS
#endif
