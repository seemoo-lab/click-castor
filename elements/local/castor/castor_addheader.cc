#include <click/config.h>
#include <click/confparse.hh>


#include "castor_addheader.hh"
#include "castor.hh"

CLICK_DECLS
CastorAddHeader::CastorAddHeader(){}

CastorAddHeader::~ CastorAddHeader(){}

int CastorAddHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorAddHeader", cpkP+cpkM, cpElementCast, "CastorFlowStub", &cflow,
        cpEnd);
}

void CastorAddHeader::push(int, Packet *p){

	//Extract source and destination from packet
    IPAddress src = p->ip_header()->ip_src.s_addr;
    IPAddress dst = p->ip_header()->ip_dst.s_addr;

	//Add Space for the new Header
	//uint32_t length = sizeof(Castor_PKT) - sizeof(click_ip);
    uint32_t length = sizeof(Castor_PKT);
	WritablePacket *q = p->push(length);
 	if (!q)
    	return;

    Castor_PKT header;
    header.type 	= CASTOR_TYPE_MERKLE_PKT;
    header.hsize 	= CASTOR_HASHLENGTH;
    header.fsize 	= CASTOR_FLOWSIZE;
    header.len 		= sizeof(Castor_PKT);

    header.ctype 	= p->ip_header()->ip_p;

    //Acces the flow settings
    PacketLabel label = cflow->getPacketLabel(src,dst);

    header.src = src;
    header.dst = dst;

    memcpy(&header.fid, &label.flow_id, CASTOR_HASHLENGTH);
    memcpy(&header.pid, &label.packet_id, CASTOR_HASHLENGTH);
    if(CASTOR_HASHLENGTH > CASTOR_ENCLENGTH) {
    	click_chatter("[Warning] Copying ACKAuth: Hash length is larger than ciphertext length, loosing entropy.");
		memcpy(&header.eauth, &label.enc_ack_auth, CASTOR_ENCLENGTH);
	} else {
		memcpy(&header.eauth, &label.enc_ack_auth, CASTOR_HASHLENGTH);
	}

 	//Copy header to packet
 	memcpy(q->data(), &header, sizeof(Castor_PKT));

	output(0).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddHeader)
