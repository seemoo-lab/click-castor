#ifndef CLICK_CASTOR_XCAST_H
#define CLICK_CASTOR_XCAST_H

#include <click/straccum.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
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
	inline void setLength(uint16_t length) { _fixed->length = length; } // FIXME: always update upon changes
	/** Indicates k-th PKT of the flow, required for flow validation (right or left siblings in Merkle tree?) */
	inline uint16_t getKPkt() const { return _fixed->kPkt; }
	inline void setKPkt(uint16_t k) { _fixed->kPkt = k; }
	inline IPAddress getSource() const { return _fixed->source; }
	inline void setSource(IPAddress source) { _fixed->source = source; }
	inline IPAddress getMulticastGroup() const { return _fixed->multicastGroup; }
	inline void setMulticastGroup(IPAddress multicastAddress) { _fixed->multicastGroup = multicastAddress; }
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
	inline IPAddress getDestination(unsigned int i) const {	return IPAddress(&_var[getDestinationOff(i)]); }
	inline void setDestination(IPAddress destination, unsigned int i) { memcpy(&_var[getDestinationOff(i)], &destination, sizeof(IPAddress)); }
	inline void setDestinations(const IPAddress destinations[], size_t n) {
		for(unsigned int i = 0; i < n; i++)
			setDestination(destinations[i], i);
		setNDestinations(n);
	}
	inline void setDestinations(const Vector<IPAddress>& destinations) { setDestinations(destinations.data(), destinations.size()); }

	inline const PacketId& getPid(unsigned int i) const { return (PacketId&) _var[getPidOff(i)]; }
	inline void setPid(const PacketId& pid, unsigned int i) { memcpy(&_var[getPidOff(i)], &pid, sizeof(PacketId)); }

	/**
	 * Removes an IP address and corresponding Pid from the destination list. Does not affect the next hop mapping, i.e., the mapping might be outdated after this call.
	 * Returns true if the address was removed from the destination list.
	 */
	inline bool removeDestination(IPAddress destination) {
		bool found = false;
		for(unsigned int i = 0; i < getNDestinations(); i++) {
			found = destination == getDestination(i);
			if(found) {
				// Cache old pids
				Vector<PacketId> pids;
				for(unsigned int j = 0; j < getNDestinations(); j++)
					if(j != i) // omit pid from destination being removed
						pids.push_back(getPid(j));

				// Copy last destination in list to this position
				setDestination(getDestination(getNDestinations() - 1), i);
				setNDestinations(getNDestinations() - 1);

				// Write pids back
				for(unsigned int j = 0; j < getNDestinations(); j++)
					setPid(pids[j], j);

				break;
			}
		}
		return found;
	}

	inline void setSingleDestination(unsigned int i) {
		IPAddress destination = getDestination(i);
		PacketId pid;
		memcpy(&pid, getPid(i), sizeof(PacketId));
		setNDestinations(1);
		setDestination(destination, 0);
		setPid(pid, 0);
	}


	inline IPAddress getNextHop(unsigned int j) const { return IPAddress(&_var[getNextHopOff(j)]); }
	inline void setNextHop(IPAddress nextHop, unsigned int j) { memcpy(&_var[getNextHopOff(j)], &nextHop, sizeof(IPAddress));}

	inline void getNextHopDestintaions(unsigned int j, Vector<unsigned int>& destinations) const {
		unsigned int off = 0;
		for(unsigned int i = 0; i < j; i++)
			off += getNextHopNAssign(i);

		for(unsigned int pos = 0; pos < getNextHopNAssign(j); pos++)
			destinations.push_back(pos + off);
	}

	inline uint8_t getNextHopNAssign(unsigned int j) const { return _var[getNextHopNAssignOff(j)]; }
	inline void setNextHopAssign(uint8_t assign, unsigned int j) { _var[getNextHopNAssignOff(j)] = assign; }

	inline void setSingleNextHop(IPAddress nextHop) {
		setNNextHops(1);
		setNextHopAssign(getNDestinations(), 0);
		setNextHop(nextHop, 0);
	}

	/**
	 * Sets new next hops.
	 * Parameter includes a mapping of next hops to destination indices.
	 */
	void setNextHopMapping(const HashTable<IPAddress,Vector<unsigned int> >& map) {
		setNNextHops(map.size());

		// XXX use swapping instead

		// Cache old destinations and pids
		Vector<IPAddress> oldDestinations;
		Vector<PacketId> oldPids;
		for(int unsigned i = 0; i < getNDestinations(); i++) {
			oldDestinations.push_back(getDestination(i));
			oldPids.push_back(getPid(i));
		}

		unsigned int hopPos = 0, dstOff = 0;
		for(HashTable<IPAddress,Vector<unsigned int> >::const_iterator it = map.begin(); it.live(); hopPos++, it++) {
			const Vector<unsigned int>& destinations = it.value();
			setNextHop(it.key(), hopPos);
			setNextHopAssign(destinations.size(), hopPos);
			for(unsigned int i = 0; i < getNextHopNAssign(hopPos); i++, dstOff++) {
				setDestination(oldDestinations[destinations[i]], dstOff);
				setPid(oldPids[destinations[i]], dstOff);
			}
		}
	}

	/*
	 * XXX method that takes a mapping and writes everything to the packet,
	 * when resizing packet, use _p->pull() and _p->push() (depending on whether packet size increases or decreases)
	 * otherwise, packet payload (partly) overwritten (if header size increases -> very bad)
	 * or padding occurs (if header size decreases -> bad for efficiency)
	 * Bad thing: always have to rewrite PKT header... but what else can we do?
	 */

	/**
	 * Return the size of the fixed header fields
	 */
	inline static size_t getFixedSize() {
		return sizeof(FixedSizeHeader);
	}

	inline size_t getSize() {
		return getFixedSize()
				+ getNDestinations() * (sizeof(IPAddress) + sizeof(PacketId))
				+ getNNextHops() * (sizeof(IPAddress) + sizeof(uint8_t));
	}

	StringAccum toString(bool full = false) {
		StringAccum sa;
		if(full) {
			String sfid = CastorPacket::hexToString(getFlowId(), getHashSize());
			String sauth = CastorPacket::hexToString(getAckAuth(), getHashSize());
			sa << "   | From:\t" << _p->dst_ip_anno() << "\n";
			sa << "   | Type:\tXcast PKT (" <<  getSize() << ")\n";
			sa << "   | Flow:\t" << getSource() << " -> " << getMulticastGroup() << "\n";
			for(unsigned int i = 0; i < getNDestinations(); i++)
				sa << "   | \t\t -> " << getDestination(i) << " (pid " << CastorPacket::hexToString(getPid(i), getHashSize()) << ")\n";
			sa << "   | Flow ID:\t" << sfid << "\n";
			sa << "   | Pkt Num: \t" << (getKPkt() + 1) << "/" << (1 << getNFlowAuthElements()) << "\n";
			sa << "   | Ack Auth:\t" << sauth << "\n";
			sa << "   | Next Hops:\t";
			unsigned int i = 0;
			sa << getNextHop(0) << " -> ";
			for(; i < getNextHopNAssign(0); i++)
				sa << getDestination(i) << ", ";
			sa << "\n";
			for(int j = 1; j < getNNextHops(); j++) {
				sa << "   | \t\t" << getNextHop(j) << " -> ";
				unsigned int off = i;
				for(; i < off + getNextHopNAssign(j); i++)
					sa << getDestination(i) << ", ";
				sa << "\n";
			}
		} else {
			sa << "Xcast PKT (from " << _p->dst_ip_anno() << ", flow " << getSource() << " -> ";
			sa << getDestination(0);
			for(unsigned int i = 0; i < getNDestinations(); i++)
				sa << ", " << getDestination(i);
			sa << ")";
		}
		return sa;
	}

	void print(bool full) {
		click_chatter("%s", toString(full).c_str());
	}

private:
	struct FixedSizeHeader {
		uint8_t type;
		uint8_t hashSize;
		uint8_t nFlowAuthElements;
		uint8_t contentType;
		uint16_t length;
		uint16_t kPkt;
		IPAddress source;
		IPAddress multicastGroup;
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
	inline unsigned int getNextHopNAssignOff(unsigned int i) const { return getNextHopOff(getNNextHops()) + sizeof(uint8_t) * i; }
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
