#include <click/config.h>
#include <click/confparse.hh>
#include <clicknet/ether.h>
#include <click/error.hh>
#include "castor_neighbours.hh"

CLICK_DECLS
CastorNeighbours::CastorNeighbours()
{}

CastorNeighbours::~ CastorNeighbours()
{}

int CastorNeighbours::configure(Vector<String> &conf, ErrorHandler *errh) {
	//if (cp_va_kparse(conf, this, errh, "MAXPACKETSIZE", cpkM, cpInteger, &maxSize, cpEnd) < 0) return -1;
	//if (maxSize <= 0) return errh->error("maxsize should be larger than 0");
	return 0;
}

bool CastorNeighbours::isNeighbour(const EtherAddress & address){
	NeighbourMap::Pair* pair = neighbours.find_pair(address);
	if (!pair) return false;
	return true;
}

void CastorNeighbours::updateNeighbour(const EtherAddress & address){
	//Check if the neighbour is already in the neighbours list
	if(isNeighbour(address)){
		click_chatter("Neighbour already known");
	}
	else{
		//Insert the new Neighbour the database
		neighbours.insert(address,1);
		click_chatter(String("Insert new Neighbour" + address.unparse()).c_str ());
	}

}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorNeighbours)

