#ifndef CLICK_CASTOR_HH
#define CLICK_CASTOR_HH

#include <click/packet.hh>
#include "node_id.hh"
#include "hash.hh"

//#define DEBUG_ACK_SRCDST  // uncomment to add source and destination fields to ACK packets
#define DEBUG_HOPCOUNT // include (unprotected) hopcount field in packets

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
	};
};

typedef Hash FlowId;
typedef Hash PacketId;
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
private:
	uint8_t		flags;
public:
	// first PKT(s) of flow
	// 'syn' is set until the first ACK for the flow is received
	inline bool syn() const { return (flags >> 7) & 1; }
	inline void set_syn() { flags |= 1 << 7; }
	inline void unset_syn() { flags &= ~(1 << 7); }
	// request retransmission of PKT
	// TODO ARQ is deprecated and can be removed including all related Elements
	inline bool arq() const { return (flags >> 6) & 1; }
	inline void set_arq() { flags |= 1 << 6; }
	inline void unset_arq() { flags &= ~(1 << 6); }
#ifdef DEBUG_HOPCOUNT
	// hopcount
	inline uint8_t hopcount() const { return flags & 0x3f; }
	inline void set_hopcount(uint8_t hc) { flags = (flags & 0xc0) | (hc & 0x3f); }
#endif
private:
	uint8_t		flow_size;
//	uint8_t 	fsize  : 4; // = Merkle tree height = log2(number of leaves)
//	uint8_t 	fasize : 4; // = number of flow authentication elements in [0..fsize]
public:
	inline uint8_t     fsize () const    { return (flow_size >> 4) & 0x0f; }
	inline void    set_fsize (uint8_t s) { flow_size = (flow_size & 0x0f) | (s << 4); }
	inline uint8_t     fasize() const    { return flow_size & 0x0f; }
	inline void    set_fasize(uint8_t s) { flow_size = (flow_size & 0xf0) | (s & 0x0f); }
	uint16_t	kpkt; // the k-th packet of the current flow, necessary for flow validation (determines whether fauth[i] is left or right sibling in the Merkle tree)
	NodeId		src;
	NodeId		dst;
	FlowId	 	fid;
	PacketId 	pid;
	ICV			icv;
	// Nonce n;
	// included if SYN = 1
	inline const Nonce* n() const { return reinterpret_cast<const Nonce*>((uint8_t*) this + sizeof(*this)); }
	inline Nonce* n() { return reinterpret_cast<Nonce*>((uint8_t*) this + sizeof(*this)); }

	// Hash fauth[fasize];
	// where fasize = 0..fsize
	inline const Hash* fauth() const { return reinterpret_cast<const Hash*>((uint8_t*) this + sizeof(*this) + ((syn()) ? sizeof(Nonce) : 0)); }
	inline Hash* fauth() { return reinterpret_cast<Hash*>((uint8_t*) this + sizeof(*this) + ((syn()) ? sizeof(Nonce) : 0)); }

	inline unsigned int header_len() const {
		return sizeof(*this) + (syn() ? sizeof(Nonce) : 0) + (unsigned int) fasize() * hsize;
	}
	inline unsigned int payload_len() const {
		return (unsigned int) ntohs(len) - header_len();
	}
};

/**
 * The Castor acknowledgment packet (ACK)
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
	FlowId		fid;
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
};

CLICK_ENDDECLS
#endif
