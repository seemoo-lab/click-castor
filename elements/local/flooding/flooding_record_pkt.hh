#ifndef CLICK_FLOODING_RECORD_PKT_HH
#define CLICK_FLOODING_RECORD_PKT_HH

#include <click/element.hh>
#include "../castor/castor.hh"
#include "../castor/castor_xcast_destination_map.hh"
#include "../castor/castor_record_pkt.hh"

CLICK_DECLS

class FloodingRecordPkt: public CastorRecordPkt {
public:
	FloodingRecordPkt() : map(0) {}

	const char *class_name() const { return "FloodingRecordPkt"; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	CastorXcastDestinationMap* map;
};

CLICK_ENDDECLS
#endif
