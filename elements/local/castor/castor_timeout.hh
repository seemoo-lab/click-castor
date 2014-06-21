#ifndef CASTOR_TIMEOUT_HH_
#define CASTOR_TIMEOUT_HH_
#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/timer.hh>
#include "castor.hh"
#include "castor_routingtable.hh"
#include "castor_history.hh"

#define TIMEOUT_MS 500 // Timeout in milliseconds

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
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);
	void create_timer(Packet*);
	void run_timer(Timer*);

private:
	CastorRoutingTable* _table;
	CastorHistory* _history;
	HashTable<Timer*,TimeoutEntry> _timers;

};

CLICK_ENDDECLS
#endif /* CASTOR_TIMEOUT_HH_ */
