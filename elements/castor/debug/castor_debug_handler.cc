#include <click/config.h>
#include <click/args.hh>
#include <click/packet.hh>
#include "castor_debug_handler.hh"

CLICK_DECLS

CastorDebugHandler::CastorDebugHandler() { };
CastorDebugHandler::~CastorDebugHandler() { };

int CastorDebugHandler::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
		.read_mp("FLOW_MANAGER", ElementCastArg("CastorFlowManager"), flow_manager)
		.complete();
}

/*
 * Creates a new PKT with the given debug attributes set.
 */
void CastorDebugHandler::send_debug_pkt(const unsigned char* src_ip, const unsigned char* dst_ip,
				  	      int dbg, int aret, int insp, int ttl, int size) {

	// Calculates the PKT size
	int new_pkt_size = size < sizeof(CastorPkt) ? sizeof(CastorPkt) : size;
	new_pkt_size = new_pkt_size < MAX_ETHER_PKT_SIZE ? new_pkt_size : MAX_ETHER_PKT_SIZE;

	// Creates a new packet with the given size
	WritablePacket* p = Packet::make(new_pkt_size);

	if (!p) {
		return;
	}

	NodeId src(src_ip);
	NodeId dst(dst_ip);

	memset(p->data(), 0, p->length());

	// Creates a PKT and sets the necessary attributes
	CastorPkt* header = reinterpret_cast<CastorPkt*>(p->data());
	header->type = CastorType::PKT;
	header->unset_syn();
	dbg ? header->set_dbg() : header->unset_dbg();
	aret ? header->set_aret() : header->unset_aret();
	insp ? header->set_insp() : header->unset_insp();
	header->set_ttl(ttl);
	header->src = src;
	header->dst = dst;

	header->hsize = sizeof(Hash);
	header->set_fsize(0);
	header->len = htons(p->length());
	header->set_fasize(0);
	header->fid = flow_manager->getCurrentFlowId(src, dst);
	crypto->random(header->pid);
	header->kpkt = 0xffff;
	header->icv = ICV();

	pkt_size = p->length();

	start_times.set(header->pid, Timestamp::now());

	output(0).push(p);
}

/*
 * Expects a string with this convention: "<src_ip>|<dst_ip>|DBG|ARET|INSP|ttl|pkt_size|"
 * Example: "192.168.0.10|192.168.0.11|1|0|0|15|56|"
 *
 * This string is parsed and a PKT is created.
 */
int CastorDebugHandler::write_callback(const String &s, Element *e, void *vparam, ErrorHandler *errh) {
	CastorDebugHandler *fh = static_cast<CastorDebugHandler*>(e);

	char* parameter = strdup(s.printable().c_str());

	const unsigned char* src_ip = (const unsigned char*)strtok(parameter, "|");
	const unsigned char* dst_ip = (const unsigned char*)strtok(NULL, "|");
	int dbg = atoi(strtok(NULL, "|"));
	int aret = atoi(strtok(NULL, "|"));
	int insp = atoi(strtok(NULL, "|"));
	int ttl = atoi(strtok(NULL, "|"));
	int size = atoi(strtok(NULL, "|"));

	fh->send_debug_pkt(src_ip, dst_ip, dbg, aret, insp, ttl, size);
	return 0;
}

/*
 * Incoming ACK-packets are stored (as a string) in a queue and not forwarded.
 * Conevention: rtt|packet_size|mac_1:ip_1, mac_2:ip_2, ...|<
 * Example: "|975|56|08-00-27-64-0F-53:192.168.56.102,08-00-27-CC-77-50:192.168.56.101|<"
 * 	 or "|81|56|<" if no path is added
 */
Packet* CastorDebugHandler::simple_action(Packet *p) {
	String dbg_ack_str(" |");
	const CastorAck& ack = *reinterpret_cast<const CastorAck*>(p->data());
	String tmp_split(",");
	int i;
	end_time = Timestamp::now();

	Timestamp diff = end_time - start_times.get(ack.auth);
	double rtt = (double)diff.sec() * 1000.0 + (double)diff.usec() / 1000.0;

	dbg_ack_str += String(rtt) + "|";
	dbg_ack_str += String(pkt_size) + "|";

	for(i=0; i < ack.path_len; i++) {
		if(i == ack.path_len-1)
			tmp_split = String("|");
		dbg_ack_str += ack.path()[i].mac.unparse() + ":" + ack.path()[i].ip.unparse() + tmp_split;
	}

	click_chatter("dbg_ack_str: %s\n", dbg_ack_str.c_str());
	dbg_ack_str += "<";
	dbg_ack_queue.push_back(dbg_ack_str);

	p->kill();
	return NULL;
}

/*
 * Returns the last element in the queue.
 */
String CastorDebugHandler::read_callback(Element *e, void *vparam) {
	Vector<String> *queue = static_cast<Vector<String>*>(vparam);
	String tmp("");

	if (!queue->empty()) {
		tmp += queue->back();
		queue->pop_back();
	}

	return tmp;
}

/*
 * Clears the queue.
 */
String CastorDebugHandler::clear_callback(Element *e, void *vparam) {
	CastorDebugHandler *fh = static_cast<CastorDebugHandler*>(e);

	fh->start_times.clear();

	if (!fh->dbg_ack_queue.empty())
		fh->dbg_ack_queue.clear();
	return String("");
}

/*
 * Adds the callback functions to handlers, so we can communicate with
 * click via a UNIX-socket.
 */
void CastorDebugHandler::add_handlers() {
	add_read_handler("clear", clear_callback, 0);
	add_write_handler("debug", write_callback, 0);
	add_read_handler("debug", read_callback, &dbg_ack_queue);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDebugHandler)
