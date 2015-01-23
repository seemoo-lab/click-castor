#ifndef CLICK_CASTOR_TIMEOUT_HH
#define CLICK_CASTOR_TIMEOUT_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/straccum.hh>
#include "castor.hh"
#include "castor_routingtable.hh"
#include "castor_history.hh"

CLICK_DECLS

class CastorTimeout : public Element {
public:
	const char *class_name() const { return "CastorTimeout"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);
	void run_timer(Timer*);

	inline int getTimeout() const { return timeout; }

private:
	class PidTimer : public Timer {
	public:
		PidTimer(CastorTimeout *element, const PacketId pid);
		inline const PacketId &getPid() const { return pid; }
	private:
		const PacketId pid;
	};

	CastorRoutingTable* table;
	CastorHistory* history;
	int timeout;
	IPAddress myIP;

	bool verbose;
};

CLICK_ENDDECLS

#endif
