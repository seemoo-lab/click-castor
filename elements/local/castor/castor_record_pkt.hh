#ifndef CLICK_CASTOR_RECORD_PKT_HH
#define CLICK_CASTOR_RECORD_PKT_HH

#include <click/element.hh>
#include <click/timestamp.hh>
#include <click/vector.hh>
#include "castor.hh"

CLICK_DECLS

class CastorRecordPkt: public Element {
public:
	CastorRecordPkt();
	~CastorRecordPkt();
		
	const char *class_name() const { return "CastorRecordPkt"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }

	void push(int, Packet *);

    void add_handlers();
private:
	class Entry {
	public:
		Entry(const PacketId& pid, const Timestamp time = Timestamp::now()) :
			time(time) {
			memcpy(&this->pid, &pid, sizeof(PacketId));
		};
		Hash pid;
		const Timestamp time;
	};

	Vector<Entry> records;
	int32_t seq_index;

	atomic_uint32_t numPids;
	atomic_uint32_t numPkts;
	atomic_uint32_t pktAccumSize;

	atomic_uint32_t broadcastDecisions;

	static String read_handler(Element*, void*);

	struct Statistics {
		enum {
			num, // number of pids recorded (same as 'numUnique' for unicast routing)
			numUnique, // number of PKTs recorded ('numUnique' <= 'num' for Xcast routing)
			size, // accumulated size of all packets (only unique PKTs counted)
			time, // Time @ which pids were received
			broadcasts, // number of broadcasts
			unicasts, // number of unicasts
			seq_entry, // returns entries one after the other
		};
	};
};

CLICK_ENDDECLS
#endif
