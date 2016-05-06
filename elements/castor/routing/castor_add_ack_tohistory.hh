#ifndef CLICK_CASTOR_ADD_ACK_TOHISTORY_HH
#define CLICK_CASTOR_ADD_ACK_TOHISTORY_HH
#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"
#include "flow/castor_flow_table.hh"

CLICK_DECLS

/**
 * Adds an acknowledgment to the history.
 * The IP address entry depends on the configuration.
 * Uses either 'DST_ANNO' of the incoming packet, or a fixed IP address.
 */
class CastorAddAckToHistory: public Element {
public:
	const char *class_name() const	{ return "CastorAddAckToHistory"; }
	const char *port_count() const	{ return PORTS_1_1; }
	const char *processing() const	{ return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	CastorHistory* history;
	CastorFlowTable* flowtable;
};

CLICK_ENDDECLS
#endif
