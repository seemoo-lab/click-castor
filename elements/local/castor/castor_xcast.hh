#ifndef CLICK_CASTOR_XCAST_H
#define CLICK_CASTOR_XCAST_H

#include "castor.hh"

CLICK_DECLS

/**
 * Castor wrapper class for a Packet. Provides accessor methods to Castor specific fields.
 */
class CastorXcastPkt {
public:
	CastorXcastPkt(Packet* p) : _p(p), _data((unsigned char*) p->data()), _fixed((FixedSizeHeader*) p->data()), _var((unsigned char*) (p->data() + sizeof(FixedSizeHeader))) {
	}
	~CastorXcastPkt() {}

	// Fixed size header part
	inline uint8_t getType() const { return _fixed->type; }
	inline void setType(uint8_t type) const { _fixed->type = type; }
	inline uint8_t getHashSize() const { return _fixed->hashSize; }
	inline void setHashSize(uint8_t hashSize) { _fixed->hashSize = hashSize; }
	inline uint8_t getNFlowAuthElements() const { return _fixed->nFlowAuthElements; }
	inline void setNFlowAuthElements(uint8_t n) { _fixed->nFlowAuthElements = n; }
	inline uint8_t getContentType() const { return _fixed->contentType; }
	inline void setContentType(uint8_t type) { _fixed->contentType = type; }
	inline uint16_t getLength() const { return _fixed->length; }
	inline void setLength(uint16_t length) { _fixed->length = length; }
	/** Indicates k-th PKT of the flow, required for flow validation (right or left siblings in Merkle tree?) */
	inline uint16_t getKPkt() const { return _fixed->kPkt; }
	inline void setKPkt(uint16_t k) { _fixed->kPkt = k; }
	inline IPAddress getSource() const { return _fixed->source; }
	inline void getSource(IPAddress source) { _fixed->source = source; }
	inline const FlowId& getFlowId() const { return _fixed->flowId; }
	inline void setFlowId(const FlowId& fid) { memcpy(&_fixed->flowId, &fid, sizeof(FlowId)); }
	inline const FlowAuth& getFlowAuth() const { return _fixed->flowAuth; }
	inline void setFlowAuth(const FlowAuth& flowAuth) {
		for (int i = 0; i < CASTOR_FLOWAUTH_ELEM; i++)
			memcpy(&_fixed->flowAuth[i], &flowAuth[i], sizeof(Hash));
	}
	inline const ACKAuth& getAckAuth() const { return _fixed->ackAuth; }
	inline void setAckAuth(const ACKAuth& ackAuth) { memcpy(&_fixed->ackAuth, &ackAuth, sizeof(ACKAuth)); }
	/** Get the number of multicast receivers */
	inline uint8_t getNDestinations() const { return _fixed->nDestinations; }
	inline void setNDestinations(uint8_t n) { _fixed->nDestinations = n; }
	/** Get the number of intended next hops */
	inline uint8_t getNNextHops() const { return _fixed->nNextHops; }
	inline void setNNextHops(uint8_t n) { _fixed->nNextHops = n; }

	// Variable length header fields
	inline IPAddress getDestination(unsigned int i) const { return IPAddress(_var[getDestinationOff(i)]); }
	inline void setDestinations(IPAddress destinations[], size_t n) { return; /* TODO implement */ }
	inline const PacketId& getPid(unsigned int i) const { return (PacketId&) _var[getPidOff(i)]; }
	inline void setPids(PacketId pids[], size_t n) { return; /* TODO implement */ };
	inline IPAddress getNextHop(unsigned int j) const { return IPAddress(_var[getNextHopOff(j)]); }
	inline void setNextHops(IPAddress nextHops[], size_t n) { return; /* TODO implement */ }
	inline uint8_t getNextHopNAssign(unsigned int j) const { return _var[getNextHopNAssignOff(j)]; }
	inline void setNextHopNAssign(uint8_t assigns[], size_t n) { return; /* TODO implement */ }

	/*
	 * TODO method that takes a mapping and writes everything to the packet,
	 * when resizing packet, use _p->pull() and _p->push() (depending on whether packet size increases or decreases)
	 * otherwise, packet payload (partly) overwritten (if header size increases -> very bad)
	 * or padding occurs (if header size decreases -> bad for efficiency)
	 * Bad thing: always have to rewrite PKT header... but what else can we do?
	 */
private:
	struct FixedSizeHeader {
		uint8_t type;
		uint8_t hashSize;
		uint8_t nFlowAuthElements;
		uint8_t contentType;
		uint16_t length;
		uint16_t kPkt;
		IPAddress source;
		FlowId flowId;
		FlowAuth flowAuth;
		ACKAuth	ackAuth;
		uint8_t nDestinations;
		uint8_t nNextHops;
	};

	Packet* _p;
	unsigned char* _data;
	FixedSizeHeader* _fixed; // Accessor to fixed fields
	unsigned char* _var; // Pointer to beginning of var fields

	inline unsigned int getDestinationOff(unsigned int i) const { return sizeof(IPAddress) * i; }
	inline unsigned int getPidOff(unsigned int i) const { return getDestinationOff(getNDestinations()) + sizeof(PacketId) * i; }
	inline unsigned int getNextHopOff(unsigned int i) const { return getPidOff(getNDestinations()) + sizeof(IPAddress) * i; }
	inline unsigned int getNextHopNAssignOff(unsigned int i) const { return getNextHopOff(getNNextHops()); }
};

// The ACK Header Structure for Explicit Multicast (Xcast)
typedef struct {
	uint8_t  	type;
	uint8_t 	esize;
	uint16_t 	len;
	EACKAuth 	auth;
} CastorXcastAck;

CLICK_ENDDECLS

#endif
