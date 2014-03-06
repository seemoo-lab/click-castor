#ifndef CLICK_CASTORNEIGHBOURS_HH
#define CLICK_CASTORNEIGHBOURS_HH
#include <click/element.hh>
#include <click/hashmap.hh>
#include <click/etheraddress.hh>
CLICK_DECLS

typedef HashMap<EtherAddress, int> NeighbourMap;

class CastorNeighbours : public Element { 
	public:
		CastorNeighbours();
		~CastorNeighbours();
		
		const char *class_name() const	{ return "CastorNeighbours"; }
		const char *port_count() const	{ return PORTS_0_0; }
		const char *processing() const	{ return AGNOSTIC; }

		int configure(Vector<String>&, ErrorHandler*);
		
		void updateNeighbour(const EtherAddress &);
		bool isNeighbour(const EtherAddress &);

	private:
		NeighbourMap neighbours;
};

CLICK_ENDDECLS
#endif

