#ifndef CLICK_FLOODING_RECORD_PKT_HH
#define CLICK_FLOODING_RECORD_PKT_HH

#include <click/element.hh>
#include "../util/castor_record_pkt.hh"
#include "../util/xcast_destination_map.hh"

CLICK_DECLS

class FloodingRecordPkt: public CastorRecordPkt {
public:
	const char *class_name() const { return "FloodingRecordPkt"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	XcastDestinationMap* map;
};

CLICK_ENDDECLS
#endif
