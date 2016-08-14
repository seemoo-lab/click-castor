#ifndef CLICK_CASTOR_DEBUG_HANDLER_HH
#define CLICK_CASTOR_DEBUG_HANDLER_HH

#include <click/element.hh>
#include "../castor.hh"
#include <vector>
#include "../flow/castor_flow_manager.hh"
#include "../flow/castor_flow_table.hh"
#include "../crypto/crypto.hh"
#include <ctime>

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

	static int write_callback(const String &s, Element *e, void *vparam, ErrorHandler *errh);
	static String read_callback(Element *e, void *vparam);
	static String clear_callback(Element *e, void *vparam);
	Packet* simple_action(Packet*);

	void add_handlers();

private:
	long ip_to_hex(const unsigned char* ip);
	void hex_to_ip(uint32_t hex_ip, char* ip_str);
	Hash rand_pid();
	Packet* create_castor_pkt(const unsigned char* src_ip, const unsigned char* dst_ip,
					int dbg, int aret, int insp, int ttl, int size);
	std::vector<String> dbg_ack_queue;
	Crypto* crypto;
};

CLICK_ENDDECLS

#endif
