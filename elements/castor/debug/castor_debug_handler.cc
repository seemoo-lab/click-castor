#include <click/config.h>
#include <click/args.hh>
#include <click/packet.hh>
#include <clicknet/udp.h>
#include <clicknet/ether.h>
#include "castor_debug_handler.hh"
#include "../castor.hh"
#include "../hash.hh"
#include <string>
#include <string.h>
#include <sstream>
#include <click/vector.hh>
#include "../flow/castor_flow.hh"
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <chrono>
#include <sys/time.h>

CLICK_DECLS

CastorDebugHandler::CastorDebugHandler() { };
CastorDebugHandler::~CastorDebugHandler() { };

int CastorDebugHandler::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
		.read_mp("FLOW_MANAGER", ElementCastArg("CastorFlowManager"), flow_manager)
		// FIXME read Crypto
		.complete();
}

/*
 * Converts an ip-address to a correspinding hex
 * Example: 192.168.56.101 => c0a83865
 */
long CastorDebugHandler::ip_to_hex(const unsigned char* ip) {
	char* dump = strdup(reinterpret_cast<const char*>(ip));
	char* b0 = strtok(dump, ".");
	char* b1 = strtok(NULL, ".");
	char* b2 = strtok(NULL, ".");
	char* b3 = strtok(NULL, ".");

	int size = sizeof(b0) + sizeof(b1) + sizeof(b2) + sizeof(b3);

	char* tmp = (char*)malloc(size);

	sprintf(tmp, "%02x%02x%02x%02x", atoi(b3), atoi(b2), atoi(b1), atoi(b0));

	long hex_ip = (long)strtol(tmp, NULL, 16);

	free(tmp);

	return hex_ip;
}

/*
 * Converts a hex to an ip-address
 * Example: c0a83865 => 192.168.56.101
 */
void CastorDebugHandler::hex_to_ip(uint32_t hex_ip, char* ip_str) {
	uint32_t tmp1 = (hex_ip & 0xff000000) >> 24;
	uint32_t tmp2 = (hex_ip & 0xff0000) >> 16;
	uint32_t tmp3 = (hex_ip & 0xff00) >> 8;
	uint32_t tmp4 = hex_ip & 0xff;
	sprintf(ip_str, "%d.%d.%d.%d", tmp1, tmp2, tmp3, tmp4);
}

/*
 * Calculates a random Hash.
 */
Hash CastorDebugHandler::rand_pid() {
	uint8_t r[CASTOR_HASH_LENGTH];
	for(int i=0; i < CASTOR_HASH_LENGTH; i++) {
		r[i] = (uint8_t) std::rand()%128;
	}
	Hash myHash(r);
	return myHash;
}

/*
 * Creates a new PKT with the given debug attributes set.
 */
void CastorDebugHandler::send_debug_pkt(const unsigned char* src_ip, const unsigned char* dst_ip,
				  	      int dbg, int aret, int insp, int ttl, int size) {

	// Calculates the PKT size
	int new_pkt_size = size < sizeof(CastorPkt) ? sizeof(CastorPkt) : size;
	new_pkt_size = new_pkt_size < MAX_ETHER_PKT_SIZE ? new_pkt_size : MAX_ETHER_PKT_SIZE;
	click_chatter("SIZE: %d\n", new_pkt_size);

	// Creates a new packet with the given size
	WritablePacket* p = Packet::make(new_pkt_size);

	if (!p) {
		click_chatter("Can not create packet!");
		return;
	}

	std::srand(std::time(0));

	NodeId src(ip_to_hex(src_ip));
	NodeId dst(ip_to_hex(dst_ip));

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
	click_chatter("createDebugPkt: pid = %lx", header->pid);

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
	click_chatter("write_callback: %s\n", s.printable().c_str());
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
	start_times.erase(ack.auth);
	double rtt = diff.sec() * 1000 + (double) diff.usec() / 1000;

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
	std::vector<String> *queue = static_cast<std::vector<String>*>(vparam);
//	String tmp("|");
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
