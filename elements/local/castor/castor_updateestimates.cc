#include <click/config.h>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "castor_updateestimates.hh"
#include <click/etheraddress.hh>
#include "castor.hh"
#include "crypto.hh"


CLICK_DECLS
CastorUpdateEstimates::CastorUpdateEstimates(){}

CastorUpdateEstimates::~ CastorUpdateEstimates(){}

int CastorUpdateEstimates::configure(Vector<String> &conf, ErrorHandler *errh) {
    return cp_va_kparse(conf, this, errh,
		"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &_table,
		"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &_history,
		"Crypto", cpkP+cpkM, cpElementCast, "Crypto", &_crypto,
        cpEnd);
}

void CastorUpdateEstimates::push(int, Packet *p){
	IPAddress nextHop;

	//Get the Packet Header
	Castor_ACK header;
	CastorPacket::getCastorACKHeader(p, &header);

    // The Address Annotation should contain the Source IP
    IPAddress src = p->dst_ip_anno();

    // Find the Packet for this ACK
    Hash pid;
    _crypto->hash(&pid, header.auth, sizeof(Hash));
    Packet* pkt = _history->getPacketById(pid);

    if(!pkt){
    	// We have never seen a packet for this ACK before
    	click_chatter("Unknown ACK, discarding");
    	p->kill();
    	return;
    }

    Castor_PKT pkt_head;
    CastorPacket::getCastorPKTHeader(pkt, &pkt_head);

    // Check if we have uni or broadcasted the packet
    if(pkt->dst_ip_anno() == IPAddress::make_broadcast()){
    	// The Packet has been broadcasted, check if we already
    	// received an ACK for that package
    	if(_history->hasACK(pkt_head.pid)){
    		//The Packet has been acked before
    		//click_chatter("Packet has been acked before,");
    		_table->updateEstimates(pkt_head.fid,src, increase, all);
    	}else{
    		//This is the first ACK for the Packet
    		//click_chatter("Receiving first ack for packet");
        	_table->updateEstimates(pkt_head.fid,src, increase, first);
        	_table->updateEstimates(pkt_head.fid,src, increase, all);

    	}
    } else if(src == pkt->dst_ip_anno()){
    	//The Packet has been unicasted, increase all estimates
    	//click_chatter("Receiving ack for unicasted packet");
    	_table->updateEstimates(pkt_head.fid,src, increase, first);
    	_table->updateEstimates(pkt_head.fid,src, increase, all);
    } else{
    	// Received ACK from unknwon source
        StringAccum sa;
        sa << "Received ACK from  " << src << " but packet was routed to " << pkt->dst_ip_anno();
        click_chatter(sa.c_str());
    	p->kill();
    	return;
    }

    //StringAccum sa;
    //sa << "Updated estimates for target " << src;
    //click_chatter(sa.c_str());


    output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateEstimates)
