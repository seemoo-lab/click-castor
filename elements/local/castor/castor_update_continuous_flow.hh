#ifndef CLICK_CASTOR_UPDATE_CONTINUOUS_FLOW_HH
#define CLICK_CASTOR_UPDATE_CONTINUOUS_FLOW_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"
#include "castor_next_flow_table.hh"
#include "crypto/crypto.hh"

CLICK_DECLS

class CastorUpdateContinuousFlow: public Element {
public:
	CastorUpdateContinuousFlow() : fidTable(NULL), history(NULL), crypto(NULL) {}

	const char *class_name() const { return "CastorUpdateContinuousFlow"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet*);
private:
	CastorNextFlowTable* fidTable;
	CastorHistory* history;
	Crypto* crypto;
};

CLICK_ENDDECLS

#endif
