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
#include <ctime>

CLICK_DECLS

CastorDebugHandler::CastorDebugHandler() { };
CastorDebugHandler::~CastorDebugHandler() { };

static CastorFlowManager* flow_manager;
static int pkt_size = 0;

int CastorDebugHandler::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
		.read_mp("FLOW_MANAGER", ElementCastArg("CastorFlowManager"), flow_manager)
		// FIXME read Crypto
		.complete();
}

// Converts an ip-address to a correspinding hex
// Example: 192.168.56.101 => c0a83865
long ip_to_hex(const unsigned char* ip) {
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

void hex_to_ip(uint32_t hex_ip, char* ip_str) {
	//char ip_str[16];
	uint32_t tmp1 = (hex_ip & 0xff000000) >> 24;
	uint32_t tmp2 = (hex_ip & 0xff0000) >> 16;
	uint32_t tmp3 = (hex_ip & 0xff00) >> 8;
	uint32_t tmp4 = hex_ip & 0xff;
	sprintf(ip_str, "%d.%d.%d.%d", tmp1, tmp2, tmp3, tmp4);
	//return ip_str;
}

Hash rand_pid() {
	uint8_t r[CASTOR_HASH_LENGTH];
	for(int i=0; i < CASTOR_HASH_LENGTH; i++) {
		r[i] = (uint8_t) std::rand()%128;
	}
	Hash myHash(r);
	return myHash;
}

Packet* CastorDebugHandler::create_castor_pkt(const unsigned char* src_ip, const unsigned char* dst_ip,
				int dbg, int aret, int insp, int size) {

	int new_pkt_size = size < sizeof(CastorPkt) ? sizeof(CastorPkt) : size;
	new_pkt_size = new_pkt_size < MAX_ETHER_PKT_SIZE ? new_pkt_size : MAX_ETHER_PKT_SIZE;

	WritablePacket* p = Packet::make(new_pkt_size);

	if (!p) {
		click_chatter("Can not create packet!");
		return NULL;
	}

	std::srand(std::time(0));

	NodeId src(ip_to_hex(src_ip));
	NodeId dst(ip_to_hex(dst_ip));

	memset(p->data(), 0, p->length());

	CastorPkt* header = reinterpret_cast<CastorPkt*>(p->data());
	header->type = CastorType::PKT;
	header->unset_syn();
	dbg ? header->set_dbg() : header->unset_dbg();
	aret ? header->set_aret() : header->unset_aret();
	insp ? header->set_insp() : header->unset_insp();
	header->src = src;
	header->dst = dst;

	//CastorFlow* flow = flow_manager->createFlowIfNotExists(header->src, header->dst);
	//PacketLabel label = flow->freshLabel();
	PacketLabel label = flow_manager->getPacketLabel(header->src, header->dst);
	unsigned int fasize = 0;
	header->hsize = sizeof(Hash);
	header->set_fsize(0);
	header->len = htons(p->length());
	header->set_fasize(0);
	header->fid = label.fid;//Hash(); // FIXME new method: flow->getCurrentFlowId(src, dst)
	crypto->random(header->pid);
	header->kpkt = 0xffff;
	header->icv = ICV();
	click_chatter("createDebugPkt: pid = %lx", header->pid);

	return p;
}

static std::clock_t start_time;

// Expects a string with this convention: "<src_ip>|<dst_ip>|DBG|ARET|INSP|pkt_size|"
// Example: "192.168.0.10|192.168.0.11|1|0|0|64|"
int CastorDebugHandler::write_callback(const String &s, Element *e, void *vparam, ErrorHandler *errh) {
	click_chatter("write_callback: %s\n", s.printable().c_str());
	CastorDebugHandler *fh = static_cast<CastorDebugHandler*>(e);

	char* parameter = strdup(s.printable().c_str());

	const unsigned char* src_ip = (const unsigned char*)strtok(parameter, "|");
	const unsigned char* dst_ip = (const unsigned char*)strtok(NULL, "|");
	int dbg = atoi(strtok(NULL, "|"));
	int aret = atoi(strtok(NULL, "|"));
	int insp = atoi(strtok(NULL, "|"));
	int size = atoi(strtok(NULL, "|"));

	Packet* p = fh->create_castor_pkt(src_ip, dst_ip, dbg, aret, insp, size);
	pkt_size = p->length();

	start_time = p->timestamp_anno().msec();

	fh->output(0).push(p);
}

// Incoming ACK-packets are stored (as a string) in a queue and not forwarded.
// Conevention: rtt|[mac_1, mac_2, ...]|
// Example: "324|080027CC7750|" or "324||" if no path is added
Packet*  CastorDebugHandler::simple_action(Packet *p) {
	String dbg_ack_str("");
	const CastorAck& ack = *reinterpret_cast<const CastorAck*>(p->data());
	String tmp_split(",");
	int i;

	//dbg_ack_str += String(p->timestamp_anno().msec()) + "|"; // sec
	//double rtt = (std::clock() - start_time) / (double) (CLOCKS_PER_SEC/1000);
	click_chatter("start_time=%lf , curr_time=%lf\n", start_time, p->timestamp_anno().msec());
	double rtt = (p->timestamp_anno().msec() - start_time);
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

// Returns the last element in the queue.
String CastorDebugHandler::read_callback(Element *e, void *vparam) {
	std::vector<String> *queue = static_cast<std::vector<String>*>(vparam);
	String tmp("|");
	if (!queue->empty()) {
		tmp += queue->back();
		queue->pop_back();
	}
	return tmp;
}

void CastorDebugHandler::add_handlers() {
	add_write_handler("debug", write_callback, 0);
	add_read_handler("debug", read_callback, &dbg_ack_queue);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorDebugHandler)
