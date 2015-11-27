#ifndef CLICK_FORWARDER_LIST_HH
#define CLICK_FORWARDER_LIST_HH

#include "../../neighbordiscovery/neighbor_id.hh"

CLICK_DECLS

class ForwarderList {
public:
	ForwarderList() : num(1), _(0) {}
	uint8_t    num;  /* number of nodes in the list, can be extended in the future */
	uint8_t    _;    /* RESERVED/PADDING */
	NeighborId node; /* forwarding node */
} CLICK_SIZE_PACKED_ATTRIBUTE;

static_assert(sizeof(ForwarderList) == 8, "ForwarderList has wrong size");

CLICK_ENDDECLS
#endif
