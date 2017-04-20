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

#ifndef CLICK_CASTOR_ANNO_HH
#define CLICK_CASTOR_ANNO_HH

#include <click/packet_anno.hh>
#include "neighbor_id.hh"
#include "hash.hh"

CLICK_DECLS

/**
 * Utility class to handle custom annotations
 */
class CastorAnno {
private:
	CastorAnno() {
		static_assert(castor_paint_offset + sizeof(uint8_t) < Packet::anno_size, "CastorAnno may access annotation space beyond Packet::anno_size");
	}
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
