#ifndef CLICK_CASTOR_ANNO_HH
#define CLICK_CASTOR_ANNO_HH

#include <click/packet_anno.hh>
#include "../neighbordiscovery/neighbor_id.hh"
#include "hash.hh"

CLICK_DECLS

/**
 * Utility class to handle custom annotations
 */
class CastorAnno {
public:
	static inline NeighborId& dst_id_anno(Packet* p) {
		return anno<NeighborId>(p, dst_id_anno_offset);
	}
	static inline NeighborId& src_id_anno(Packet* p) {
		return anno<NeighborId>(p, src_id_anno_offset);
	}
	static inline NeighborId& hop_id_anno(Packet* p) {
		return anno<NeighborId>(p, hop_id_anno_offset);
	}
	static inline NeighborId& id_anno(Packet* p, uint8_t offset) {
		return anno<NeighborId>(p, offset);
	}
	static inline Hash& hash_anno(Packet* p) {
		return anno<Hash>(p, castor_anno_offset);
	}

	static const uint8_t src_id_anno_offset = 0;
	static const uint8_t dst_id_anno_offset = src_id_anno_offset + sizeof(NeighborId);
	static const uint8_t hop_id_anno_offset = dst_id_anno_offset + sizeof(NeighborId);
	static const uint8_t castor_anno_offset = hop_id_anno_offset + sizeof(NeighborId);
	static const uint8_t castor_paint_offset = castor_anno_offset + sizeof(Hash);
	static_assert(castor_paint_offset + sizeof(uint8_t) < Packet::anno_size, "CastorAnno may access annotation space beyond Packet::anno_size");
private:
	template<typename T>
	static inline T& anno(Packet* p, uint8_t offset) {
		assert(offset + sizeof(T) < Packet::anno_size);
		auto* anno_ptr = p->anno_u8();
		anno_ptr += offset;
		return reinterpret_cast<T&>(*anno_ptr);
	}
};

CLICK_ENDDECLS

#endif
