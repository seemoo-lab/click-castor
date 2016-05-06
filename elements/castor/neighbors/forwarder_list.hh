#ifndef CLICK_FORWARDER_LIST_HH
#define CLICK_FORWARDER_LIST_HH

#include "../neighbor_id.hh"

CLICK_DECLS

class ForwarderList {
public:
	ForwarderList() : num(1), nicv(1) {}
	uint8_t    num;  /* number of node identifiers in the list, can be extended in the future */
	uint8_t    nicv; /* number of ICVs appended to this packet,	num != nicv if 'node'
						is the broadcast address, i.e., more nodes addressed explicitly enumerated */
	NeighborId node; /* forwarding node */
} CLICK_SIZE_PACKED_ATTRIBUTE;

static_assert(sizeof(ForwarderList) == 8, "ForwarderList has wrong size");

CLICK_ENDDECLS
#endif
