#ifndef CLICK_CASTOR_XCAST_HH
#define CLICK_CASTOR_XCAST_HH

#include <click/straccum.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
#include "castor.hh"
#include "castor_anno.hh"
#include "group_id.hh"

CLICK_DECLS

/**
 * Xcastor wrapper class for a data packet. Provides accessor methods to Xcastor specific fields.
 */
class CastorXcastPkt {
public:
	/**
	 * Creates an Xcast PKT. Call this on a Packet that already contains a valid Xcast header at 'p->data()'.
	 */
	CastorXcastPkt(Packet* p) {
		set_internal_pointers(p);
	}

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
#ifdef DEBUG_HOPCOUNT
		pkt._fixed->hopcount = 0;
#endif
		pkt._fixed->length = sizeof(FixedSizeHeader);

		return pkt;
	}

	Packet* getPacket() { return _p; }

	// Fixed size header part
	inline uint8_t& type() { return _fixed->type; }
	inline uint8_t& hash_size() { return _fixed->hashSize; }
	inline uint8_t& flow_size() { return _fixed->nFlowAuthElements; }
	inline uint8_t& content_type() { return _fixed->contentType; }
	inline uint16_t& header_length() { return _fixed->length; }
#ifdef DEBUG_HOPCOUNT
	inline uint8_t& hopcount() { return _fixed->hopcount; }
#endif
	/** Indicates k-th PKT of the flow, required for flow validation (right or left siblings in Merkle tree?) */
	inline uint16_t& kpkt() { return _fixed->kPkt; }
	inline NodeId& src() { return _fixed->source; }
	inline GroupId& dst_group() { return _fixed->multicastGroup; }
	inline FlowId& fid() { return _fixed->flowId; }
	inline FlowAuth& flow_auth() { return _fixed->flowAuth; }
	inline PktAuth& pkt_auth() { return _fixed->ackAuth; }
	/** Get the number of multicast receivers */
	inline const uint8_t ndst() const { return _fixed->nDestinations; }
	inline void set_ndst(uint8_t n) { _fixed->nDestinations = n; set_length(); }
	/** Get the number of intended next hops */
	inline uint8_t nnexthop() const { return _fixed->nNextHops; }
	inline void set_nnexthop(uint8_t n) { _fixed->nNextHops = n; set_length(); }

	// Variable length header fields
	inline NodeId& dst(uint8_t i) { return reinterpret_cast<NodeId&>(*(_var + dst_off(i))); }
	/** After method returns, next hops have to be newly set **/
	inline void set_dsts(const NodeId destinations[], size_t n) {
		set_ndst(n);
		for(unsigned int i = 0; i < n; i++)
			dst(i) = destinations[i];
	}
	inline void set_dsts(const Vector<NodeId>& destinations) { set_dsts(destinations.data(), destinations.size()); }

	inline PacketId& pid(uint8_t i) { return reinterpret_cast<PacketId&>(*(_var + pid_off(i))); }

	/**
	 * Removes an IP address and corresponding Pid from the destination list. Does not affect the next hop mapping, i.e., the mapping might be outdated after this call.
	 * Returns true if the address was removed from the destination list.
	 */
	void remove(const NodeId& destination) {
		uint8_t index = dst_index(destination);
		if (index < 0)
			return;
		HashTable<uint8_t,uint8_t> map;
		map.set(index, index);
		remove(map);
	}

	/**
	 * Find index of specified destination node
	 * Returns -1 if not found
	 */
	uint8_t dst_index(const NodeId& node) {
		for(uint8_t i = 0; i < ndst(); i++)
			if (dst(i) == node)
				return i;
		return -1;
	}

	/**
	 * Remove destinations at specified indices (keep all others)
	 *
	 * Warning: invalidates next hop assignment
	 */
	inline void remove(const HashTable<uint8_t, uint8_t>& dst_indices) {
		if (dst_indices.empty())
			return;

		uint8_t new_ndst = ndst() - dst_indices.size();

		NodeId   dsts[new_ndst];
		PacketId pids[new_ndst];
		for(uint8_t i = 0, j = 0; i < ndst(); i++) {
			if(dst_indices.count(i) == 0) {
				dsts[j] = dst(i);
				pids[j] = pid(i);
				j++;
			}
		}

		set_ndst(new_ndst);

		for(uint8_t j = 0; j < new_ndst; j++) {
			dst(j) = dsts[j];
			pid(j) = pids[j];
		}
	}

	/**
	 * Keep destinations at specified indices (remove all others)
	 *
	 * Warning: invalidates next hop assignment
	 */
	inline void keep(const HashTable<uint8_t, uint8_t>& dst_indices) {
		if (dst_indices.empty())
			return;

		uint8_t new_ndst = dst_indices.size();

		NodeId   dsts[new_ndst];
		PacketId pids[new_ndst];
		for(uint8_t i = 0, j = 0; i < ndst(); i++) {
			if(dst_indices.count(i) == 1) {
				dsts[j] = dst(i);
				pids[j] = pid(i);
				j++;
			}
		}

		set_ndst(new_ndst);

		for(uint8_t j = 0; j < new_ndst; j++) {
			dst(j) = dsts[j];
			pid(j) = pids[j];
		}
	}

	/**
	 * Warning: invalidates next hop assignment
	 */
	inline void set_single_dst(uint8_t i) {
		assert(i < ndst());
		NodeId   tmp_dst = dst(i);
		PacketId tmp_pid = pid(i);
		set_ndst(1);
		dst(0) = tmp_dst;
		pid(0) = tmp_pid;
	}

	inline NeighborId& nexthop(uint8_t j) const { return reinterpret_cast<NeighborId&>(*(_var + nexthop_off(j))); }

	inline void nexthop_assigned_dsts(uint8_t j, Vector<unsigned int>& destinations) const {
		uint8_t off = 0;
		for(uint8_t i = 0; i < j; i++)
			off += nexthop_assign(i);
		for(uint8_t pos = 0; pos < nexthop_assign(j); pos++)
			destinations.push_back(pos + off);
	}

	inline uint8_t& nexthop_assign(unsigned int j) const { return _var[nexthop_assign_off(j)]; }

	inline void set_single_nexthop(const NeighborId& node) {
		set_nnexthop(1);
		nexthop(0) = node;
		nexthop_assign(0) = ndst();
	}

	/**
	 * Sets new next hops.
	 * Parameter includes a mapping of next hops to destination indices.
	 */
	void set_nexthop_assign(const HashTable<NeighborId,Vector<unsigned int> >& map) {
		set_nnexthop(map.size());

		NodeId   old_dsts[ndst()];
		PacketId old_pids[ndst()];
		for(uint8_t i = 0; i < ndst(); i++) {
			old_dsts[i] = dst(i);
			old_pids[i] = pid(i);
		}

		uint8_t hop = 0, dst_off = 0;
		for(const auto& entry : map) {
			const auto& dsts = entry.second;
			nexthop(hop) = entry.first;
			nexthop_assign(hop) = dsts.size();
			for(uint8_t i = 0; i < nexthop_assign(hop); i++, dst_off++) {
				dst(dst_off) = old_dsts[dsts[i]];
				pid(dst_off) = old_pids[dsts[i]];
			}
			hop++;
		}
	}

	StringAccum toString(bool full = false) {
		StringAccum sa;
		if(full) {
			String sfid = fid().str();
			String sauth = pkt_auth().str();
			sa << "   | From:\t" << CastorAnno::src_id_anno(_p) << "\n";
			sa << "   | To:\t" << CastorAnno::dst_id_anno(_p) << "\n";
			sa << "   | Type:\tXcast PKT (header " <<  header_length() << " / payload " << (_p->length() - header_length()) << " bytes)\n";
			sa << "   | Flow:\t" << src() << " -> " << dst_group() << "\n";
			for(uint8_t i = 0; i < ndst(); i++)
				sa << "   | \t\t -> " << dst(i) << " (pid " << pid(i).str() << ")\n";
			sa << "   | Flow ID:\t" << sfid << "\n";
			sa << "   | Pkt Num: \t" << (kpkt() + 1) << "/" << (1 << flow_size()) << "\n";
			sa << "   | Ack Auth:\t" << sauth << "\n";
			sa << "   | Next Hops:\t";
			uint8_t i = 0;
			sa << nexthop(0) << " -> ";
			for(; i < nexthop_assign(0); i++)
				sa << dst(i) << ", ";
			sa << "\n";
			for(int j = 1; j < nnexthop(); j++) {
				sa << "   | \t\t" << nexthop(j) << " -> ";
				uint8_t off = i;
				for(; i < off + nexthop_assign(j); i++)
					sa << dst(i) << ", ";
				sa << "\n";
			}
		} else {
			sa << "Xcast PKT (from " << CastorAnno::src_id_anno(_p) << " to " << CastorAnno::dst_id_anno(_p) << ", flow " << src() << " -> ";
			sa << dst(0);
			for(uint8_t i = 1; i < ndst(); i++)
				sa << ", " << dst(i);
			sa << ")";
		}
		return sa;
	}

private:
	struct FixedSizeHeader {
		uint8_t type;
		uint8_t hashSize;
		uint8_t nFlowAuthElements;
		uint8_t contentType;
		uint16_t length;
		uint16_t kPkt;
		NodeId source;
		GroupId multicastGroup;
		FlowId flowId;
		FlowAuth flowAuth;
		AckAuth	ackAuth;
		uint8_t nDestinations;
		uint8_t nNextHops;
#ifdef DEBUG_HOPCOUNT
		uint8_t hopcount;
#endif
	};

	Packet* _p;
	FixedSizeHeader* _fixed; // Accessor to fixed fields
	uint8_t* _var; // Pointer to beginning of var fields

	inline unsigned int dst_off(uint8_t i) const { return sizeof(NodeId) * i; }
	inline unsigned int pid_off(uint8_t i) const { return dst_off(ndst()) + sizeof(PacketId) * i; }
	inline unsigned int nexthop_off(uint8_t i) const { return pid_off(ndst()) + sizeof(NeighborId) * i; }
	inline unsigned int nexthop_assign_off(uint8_t i) const { return nexthop_off(nnexthop()) + sizeof(uint8_t) * i; }

	void set_internal_pointers(Packet* p) {
		_p = p;
		_fixed = reinterpret_cast<FixedSizeHeader*>(const_cast<uint8_t*>(_p->data()));
		_var = const_cast<uint8_t*>(_p->data()) + sizeof(FixedSizeHeader);
	}

	/**
	 * Resizes the PKT header in front of the user data to accommodate currently set 'nDestinations' and 'nNextHops'.
	 * If shrunken, bytes are removed from the back of the header.
	 */
	void set_length() {
		size_t old_length = _fixed->length;

		size_t new_length = sizeof(FixedSizeHeader)
			+ ndst()     * (sizeof(NodeId) + sizeof(PacketId))
			+ nnexthop() * (sizeof(NeighborId) + sizeof(uint8_t));

		if(old_length == new_length)
			return; // No need to resize -> cheap

		// Expensive copy
		uint8_t copy[new_length];

		memcpy(&copy, _p->data(), new_length);

		// Remove space or add space, respectively
		WritablePacket* q;
		if(old_length > new_length) {
			_p->pull(old_length - new_length);
			q = _p->uniqueify();
		} else { // old_length < new_length
			q = _p->push(new_length - old_length);
		}
		if(!q) {
			click_chatter("Resizing header: Could not uniqueify Packet!");
			return;
		}

		memcpy(q->data(), &copy, new_length);

		set_internal_pointers(q);

		_fixed->length = new_length;
	}
};

CLICK_ENDDECLS

#endif
