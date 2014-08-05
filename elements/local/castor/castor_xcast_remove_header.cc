#include <click/config.h>
#include <click/confparse.hh>
#include "castor_xcast_remove_header.hh"
#include "castor_xcast.hh"

CLICK_DECLS

CastorXcastRemoveHeader::CastorXcastRemoveHeader() {
}

CastorXcastRemoveHeader::~CastorXcastRemoveHeader() {
}

int CastorXcastRemoveHeader::configure(Vector<String> &conf, ErrorHandler *errh) {
	unsigned int maxGroupSize;

     int result = cp_va_kparse(conf, this, errh,
        "MaxGroupSize", cpkP+cpkM, cpUnsigned, &maxGroupSize, // TODO for simpler implementation -> no need to resize packet in subsequent elements, but transmits potentially larger packets
        cpEnd);
     varSpace = maxGroupSize * (sizeof(IPAddress) + sizeof(PacketId)) + // Space for 'maxGroupSize' destinations
     	 	 	maxGroupSize * (sizeof(IPAddress) + sizeof(uint8_t));   // Space for one next hop per destination

     if(result < 0)
    	 return -1;
     return 0;
}

void CastorXcastRemoveHeader::push(int, Packet* p){
	WritablePacket *q = p->uniqueify();
	if(!q)
		return;

	q->pull(CastorXcastPkt::getFixedSize() + varSpace);

	output(0).push(q);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorXcastRemoveHeader)
