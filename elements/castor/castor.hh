/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CLICK_CASTOR_HH
#define CLICK_CASTOR_HH

#include <click/packet.hh>
#include <string>
#include <click/vector.hh>
#include "node_id.hh"
#include "hash.hh"
#include "neighbor_id.hh"

//#define DEBUG_ACK_SRCDST  // uncomment to add source and destination fields to ACK packets
//#define DEBUG_HOPCOUNT // WARNING: Can not be used with Debug! Include (unprotected) hopcount field in packets

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
	uint8_t		flags;
public:
	// first PKT(s) of flow
	// 'syn' is set until the first ACK for the flow is received
	inline bool syn() const { return flags & 0x01; }
	inline void set_syn() { flags |= 0x01; }
	inline void unset_syn() { flags &= ~0x01; }
#ifdef DEBUG_HOPCOUNT
	// hopcount
	inline uint8_t hopcount() const { return flags & 0x0f; }
	inline void set_hopcount(uint8_t hc) { flags = (flags & 0xf0) | (hc & 0x0f); }
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
} CLICK_SIZE_PACKED_ATTRIBUTE;

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
} CLICK_SIZE_PACKED_ATTRIBUTE;

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
