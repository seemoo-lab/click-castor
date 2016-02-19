#ifndef CLICK_CASTOR_CHECK_DUPLICATE_HH
#define CLICK_CASTOR_CHECK_DUPLICATE_HH

#include <click/element.hh>
#include "castor_history.hh"
#include "flow/castor_flow_table.hh"

CLICK_DECLS

class CastorCheckDuplicate : public Element {
public:
	const char *class_name() const	{ return "CastorCheckDuplicate"; }
	const char *port_count() const	{ return "1/5"; }
	const char *processing() const	{ return PROCESSING_A_AH; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet *);
private:
	CastorHistory* history;
	CastorFlowTable* flowtable;
};

CLICK_ENDDECLS

#endif
