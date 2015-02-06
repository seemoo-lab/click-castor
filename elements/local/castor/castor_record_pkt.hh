#ifndef CLICK_CASTOR_RECORD_PKT_HH
#define CLICK_CASTOR_RECORD_PKT_HH

#include <click/element.hh>
#include <click/timestamp.hh>
#include <click/list.hh>
#include <click/vector.hh>
#include "castor.hh"

CLICK_DECLS

class CastorRecordPkt: public Element {
public:
	CastorRecordPkt() : interval(Timestamp::make_sec(1)) {
		numPkts = 0, numPids = 0, pktAccumSize = 0, broadcastDecisions = 0, seq_index = 0, hopcount_index = 0;
	}
		
	const char *class_name() const { return "CastorRecordPkt"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

    void add_handlers();
private:
	class Entry {
	public:
		Entry(const PacketId& pid, size_t size = 0, const Timestamp time = Timestamp::now()) :
			pid(pid), size(size), time(time) {};
		const Hash pid;
		const size_t size;
		const Timestamp time;
	};

	struct ListNode {
		inline ListNode() { value = 0; }
		List_member<ListNode> node;
		atomic_uint32_t value;
	};

	Timestamp interval;
	Timestamp currentIntervalEnd;
	List<ListNode, &ListNode::node> sizeInterval;

	Vector<Entry> records;
	int32_t seq_index;

	Vector<uint8_t> hopcounts;
	int32_t hopcount_index;

	atomic_uint32_t numPids;
	atomic_uint32_t numPkts;
	atomic_uint32_t pktAccumSize;

	atomic_uint32_t broadcastDecisions;

	static String read_handler(Element*, void*);

	struct Statistics {
		enum {
			num, // number of pids recorded (same as 'numUnique' for unicast routing)
			numUnique, // number of PKTs recorded ('numUnique' <= 'num')
			size, // accumulated size of all packets (only unique PKTs counted)
			broadcasts, // number of recorded broadcast pids
			unicasts, // number of recorded unicast pids
			seq_entry, // returns entries one after the other
			seq_hopcount, // total hop count of received PKTs
			size_interval,
		};
	};
};

CLICK_ENDDECLS
#endif
