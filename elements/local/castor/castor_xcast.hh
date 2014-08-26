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
	/**
	 * Creates an Xcast PKT. Call this on a Packet that already contains a valid Xcast header at 'p->data()'.
	 */
	CastorXcastPkt(Packet* p) {
		setPointers(p);
	}
	~CastorXcastPkt() {}

	/**
	 * Makes room for an Xcast header and then creates an Xcast PKT.
	 */
	static CastorXcastPkt initialize(Packet* p) {
		WritablePacket* q = p->push(sizeof(FixedSizeHeader));
		CastorXcastPkt pkt(q);

		// Important to initialize those two to zero,
		// otherwise next call to setLength() will result in garbage
		pkt._fixed->nDestinations = 0;
		pkt._fixed->nNextHops = 0;

		pkt._fixed->length = sizeof(FixedSizeHeader);

		return pkt;
	}

	Packet* getPacket() { return _p; }

	// Fixed size header part
	inline uint8_t getType() const { return _fixed->type; }
	inline void setType(uint8_t type) const { _fixed->type = type; }
	inline uint8_t getHashSize() const { return _fixed->hashSize; }
	inline void setHashSize(uint8_t hashSize) { _fixed->hashSize = hashSize; }
	inline uint8_t getNFlowAuthElements() const { return _fixed->nFlowAuthElements; }
	inline void setNFlowAuthElements(uint8_t n) { _fixed->nFlowAuthElements = n; }
	inline uint8_t getContentType() const { return _fixed->contentType; }
	inline void setContentType(uint8_t type) { _fixed->contentType = type; }
	/** Returns the length of the Castor header */
	inline uint16_t getHeaderLength() const { return _fixed->length; }
	/** Returns the length of entire Packet, i.e., header and payload */
	inline uint32_t getTotalLength() const { return _p->length(); }
	/** Returns the length of the payload */
	inline uint32_t getPayloadLength() const { return getTotalLength() - getHeaderLength(); }
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
	inline void setNDestinations(uint8_t n) { _fixed->nDestinations = n; setLength(); }
	/** Get the number of intended next hops */
	inline uint8_t getNNextHops() const { return _fixed->nNextHops; }
	inline void setNNextHops(uint8_t n) { _fixed->nNextHops = n; setLength(); }

	// Variable length header fields
	inline IPAddress getDestination(unsigned int i) const {	return IPAddress(&_var[getDestinationOff(i)]); }
	inline void setDestination(IPAddress destination, unsigned int i) { memcpy(&_var[getDestinationOff(i)], &destination, sizeof(IPAddress)); }
	inline void setDestinations(const IPAddress destinations[], size_t n) {
		setNDestinations(n);
		for(unsigned int i = 0; i < n; i++)
			setDestination(destinations[i], i);
	}
	inline void setDestinations(const Vector<IPAddress>& destinations) { setDestinations(destinations.data(), destinations.size()); }

	inline const PacketId& getPid(unsigned int i) const { return (PacketId&) _var[getPidOff(i)]; }
	inline void setPid(const PacketId& pid, unsigned int i) { memcpy(&_var[getPidOff(i)], &pid, sizeof(PacketId)); }

	/**
	 * Removes an IP address and corresponding Pid from the destination list. Does not affect the next hop mapping, i.e., the mapping might be outdated after this call.
	 * Returns true if the address was removed from the destination list.
	 */
	bool removeDestination(IPAddress destination) {
		bool found = false;
		for(unsigned int i = 0; i < getNDestinations(); i++) {
			found = destination == getDestination(i);
			if(found) {
				// Cache old destinations & pids
				Vector<IPAddress> dests;
				Vector<PacketId> pids;
				for(unsigned int j = 0; j < getNDestinations(); j++)
					if(j != i) {
						// omit pid from destination being removed
						dests.push_back(getDestination(j));
						pids.push_back(getPid(j));
					}

				setNDestinations(getNDestinations() - 1);

				// Write pids back
				for(unsigned int j = 0; j < getNDestinations(); j++) {
					setDestination(dests[j], j);
					setPid(pids[j], j);
				}
				break;
			}
		}
		return found;
	}

	void removeDestinations(HashTable<uint8_t, uint8_t>& toBeRemoved) {
		Vector<IPAddress> dests;
		Vector<PacketId> pids;
		for(uint8_t i = 0; i < getNDestinations(); i++) {
			if(toBeRemoved.find(i) == toBeRemoved.end()) {
				dests.push_back(getDestination(i));
				pids.push_back(getPid(i));
			}
		}

		setNDestinations(getNDestinations() - toBeRemoved.size());

		// Write pids back
		for(unsigned int j = 0; j < getNDestinations(); j++) {
			setDestination(dests[j], j);
			setPid(pids[j], j);
		}
	}

	void keepDestinations(HashTable<uint8_t, uint8_t>& toRemain) {
		Vector<IPAddress> dests;
		Vector<PacketId> pids;
		for(uint8_t i = 0; i < getNDestinations(); i++) {
			if(toRemain.find(i) != toRemain.end()) {
				dests.push_back(getDestination(i));
				pids.push_back(getPid(i));
			}
		}

		setNDestinations(toRemain.size());

		// Write pids back
		for(unsigned int j = 0; j < getNDestinations(); j++) {
			setDestination(dests[j], j);
			setPid(pids[j], j);
		}
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

	StringAccum toString(bool full = false) {
		StringAccum sa;
		if(full) {
			String sfid = CastorPacket::hexToString(getFlowId(), getHashSize());
			String sauth = CastorPacket::hexToString(getAckAuth(), getHashSize());
			sa << "   | From:\t" << CastorPacket::src_ip_anno(_p) << "\n";
			sa << "   | To:\t" << _p->dst_ip_anno() << "\n";
			sa << "   | Type:\tXcast PKT (header " <<  getHeaderLength() << " / payload " << getPayloadLength() << " bytes)\n";
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
			sa << "Xcast PKT (from " << CastorPacket::src_ip_anno(_p) << " to " << getPacket()->dst_ip_anno() << ", flow " << getSource() << " -> ";
			sa << getDestination(0);
			for(unsigned int i = 1; i < getNDestinations(); i++)
				sa << ", " << getDestination(i);
			sa << ")";
		}
		return sa;
	}

	void print(bool full) {
		click_chatter("%s", toString(full).c_str());
	}

	inline static size_t getFixedSize() {
		return sizeof(FixedSizeHeader);
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
	FixedSizeHeader* _fixed; // Accessor to fixed fields
	uint8_t* _var; // Pointer to beginning of var fields

	inline unsigned int getDestinationOff(unsigned int i) const { return sizeof(IPAddress) * i; }
	inline unsigned int getPidOff(unsigned int i) const { return getDestinationOff(getNDestinations()) + sizeof(PacketId) * i; }
	inline unsigned int getNextHopOff(unsigned int i) const { return getPidOff(getNDestinations()) + sizeof(IPAddress) * i; }
	inline unsigned int getNextHopNAssignOff(unsigned int i) const { return getNextHopOff(getNNextHops()) + sizeof(uint8_t) * i; }

	void setPointers(Packet* p) {
		_p = p;
		_fixed = (FixedSizeHeader*) _p->data();
		_var = (unsigned char*) (_p->data() + sizeof(FixedSizeHeader));
	}

	/**
	 * Resizes the PKT header in front of the user data to accommodate currently set 'nDestinations' and 'nNextHops'.
	 * If shrunken, bytes are removed from the back of the header.
	 */
	void setLength() {
		size_t oldLength = _fixed->length;

		size_t newLength = sizeof(FixedSizeHeader)
			+ getNDestinations() * (sizeof(IPAddress) + sizeof(PacketId))
			+ getNNextHops() * (sizeof(IPAddress) + sizeof(uint8_t));

		if(oldLength == newLength)
			return; // No need to resize -> cheap

		// Expensive copy
		uint8_t copy[newLength];

		memcpy(&copy, _p->data(), newLength);

		// Remove space or add space, respectively
		WritablePacket* q;
		if(oldLength > newLength) {
			_p->pull(oldLength - newLength);
			q = _p->uniqueify();
		} else { // oldLength < newLength
			q = _p->push(newLength - oldLength);
		}
		if(!q) {
			click_chatter("Resizing header: Could not uniqueify Packet!");
			return;
		}

		memcpy(q->data(), &copy, newLength);

		// Fix pointers
		setPointers(q);

		_fixed->length = newLength;
	}
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
