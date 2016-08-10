#ifndef CLICK_CASTOR_INSERT_PATH_HH
#define CLICK_CASTOR_INSERT_PATH_HH

#include <click/element.hh>
#include "../neighbor_id.hh"
#include "../castor.hh"

CLICK_DECLS

class CastorInsertPath: public Element {
public:
	const char *class_name() const { return "CastorInsertPath"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet*);
private:
//	uint64_t mac_to_hex(String mac);
//	uint64_t getMac();
	NeighborId myMac;
	NodeId myIp;
};

CLICK_ENDDECLS

#endif
