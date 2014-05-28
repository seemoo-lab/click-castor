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

	if(!_history->ValidateACK(p)){
		click_chatter("Unknown ACK, discarding");
		p->kill();
		return;
	}

    // The Address Annotation should contain the Source IP
    IPAddress src = p->dst_ip_anno();

	// Determine Destination of the origin Packet
	IPAddress dest = _history->PKTroutedto(p);

	// Determine the Flow ID
	FlowId fid;
	_history->GetFlowId(p, &fid);

	// Check if we have uni or broadcasted the packet
	if(dest == IPAddress::make_broadcast()){
		if(_history->IsFirstACK(p)){
    		//This is the first ACK for the Packet
    		//click_chatter("Receiving first ack for packet");
        	_table->updateEstimates(fid,src, increase, first);
        	_table->updateEstimates(fid,src, increase, all);
		} else {
			_table->updateEstimates(fid,src, increase, all);
		}
	} else if(src == dest){
    	//The Packet has been unicasted, increase all estimates
    	//click_chatter("Receiving ack for unicasted packet");
    	_table->updateEstimates(fid,src, increase, first);
    	_table->updateEstimates(fid,src, increase, all);
	} else {
		// Received ACK from unknwon source
		 StringAccum sa;
		 sa << "Received ACK from  " << src << " but packet was routed to " << src;
		 click_chatter(sa.c_str());
		 p->kill();
		 return;
	}

    output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorUpdateEstimates)
