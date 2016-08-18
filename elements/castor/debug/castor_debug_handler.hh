#ifndef CLICK_CASTOR_DEBUG_HANDLER_HH
#define CLICK_CASTOR_DEBUG_HANDLER_HH

#include <click/element.hh>
#include "../castor.hh"
#include <click/vector.hh>
#include "../flow/castor_flow_manager.hh"
#include "../flow/castor_flow_table.hh"
#include "../crypto/crypto.hh"

CLICK_DECLS

#define MAX_ETHER_PKT_SIZE 1480

class CastorDebugHandler : public Element {
public:
	CastorDebugHandler();
	~CastorDebugHandler();

	const char *class_name() const { return "CastorDebugHandler"; }
	const char *port_count() const { return "1/1"; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	// Incoming ACK-packets are stored (as a string) in a queue and not forwarded
	Packet* simple_action(Packet*);

	// Expects a string with this convention: "<src_ip>|<dst_ip>|DBG|ARET|INSP|ttl|pkt_size|"
	// This string is parsed and a PKT is created
	static int write_callback(const String &s, Element *e, void *vparam, ErrorHandler *errh);

	// Returns the last element in the queue
	static String read_callback(Element *e, void *vparam);

	// Clears the queue
	static String clear_callback(Element *e, void *vparam);

	// Adds the callback functions to handlers, so we can communicate with
	// click via a UNIX-socket.
	void add_handlers();

private:
	// Converts an ip-address to a correspinding hex
	long ip_to_hex(const unsigned char* ip);

	// Converts a hex to an ip-address
	void hex_to_ip(uint32_t hex_ip, char* ip_str);

	// Calculate a random Hash
	Hash rand_pid();

	// Creates a new PKT with the given debug attributes set
	void send_debug_pkt(const unsigned char* src_ip, const unsigned char* dst_ip,
					int dbg, int aret, int insp, int ttl, int size);

	// Stores all incoming ACK as a string
	Vector<String> dbg_ack_queue;

	Crypto* crypto;
	CastorFlowManager* flow_manager;

	int pkt_size;

	// Stores, for each PKT, the time when it is send
	HashTable<PacketId, Timestamp> start_times; 

	// Stores the time when the ACK is received 
	Timestamp end_time;
};

CLICK_ENDDECLS

#endif
