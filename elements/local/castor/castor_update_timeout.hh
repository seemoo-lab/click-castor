#ifndef CLICK_CASTOR_UPDATE_TIMEOUT_HH
#define CLICK_CASTOR_UPDATE_TIMEOUT_HH

#include <click/element.hh>
#include "castor_history.hh"
#include "castor_timeout_table.hh"

CLICK_DECLS

class CastorUpdateTimeout : public Element {
public:
	CastorUpdateTimeout() : table(NULL), history(NULL), verbose(false) {};

	const char *class_name() const { return "CastorUpdateTimeout"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
private:
	CastorTimeoutTable* table;
	CastorHistory* history;
	bool verbose;
};

CLICK_ENDDECLS

#endif
