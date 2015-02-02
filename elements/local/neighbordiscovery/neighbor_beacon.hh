#ifndef CLICK_NEIGHBOR_BEACON_H
#define CLICK_NEIGHBOR_BEACON_H

#include <click/ipaddress.hh>

CLICK_DECLS

#define ETHERTYPE_BEACON 0x88B5 // 0x88B5 and 0x88B6 reserved for private experiments, so we use them

typedef IPAddress NodeId;

struct NeighborBeacon {
	NeighborBeacon(NodeId src) : src(src) {}
	NodeId src; // Contains the source node
};

CLICK_ENDDECLS
#endif
