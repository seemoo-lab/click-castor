#ifndef CLICK_CASTOR_TIMEOUT_HH
#define CLICK_CASTOR_TIMEOUT_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/timer.hh>
#include "castor.hh"
#include "castor_routingtable.hh"
#include "castor_history.hh"

CLICK_DECLS

// TODO Could be merged with HistoryEntry, but would require exposure of this data type
typedef struct {
	FlowId fid;
	PacketId pid;
	IPAddress routedTo;
} TimeoutEntry;

class CastorTimeout: public Element {

public:
	CastorTimeout();
	~CastorTimeout();

	const char *class_name() const { return "CastorTimeout"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
	void run_timer(Timer*);

private:
	CastorRoutingTable* table;
	CastorHistory* history;
	HashTable<Timer*,TimeoutEntry> timers;
	int timeout;

};

CLICK_ENDDECLS
#endif /* CASTOR_TIMEOUT_HH_ */
