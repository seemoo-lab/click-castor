#ifndef CLICK_CASTOR_XCAST_DESTINATION_MAP_HH
#define CLICK_CASTOR_XCAST_DESTINATION_MAP_HH

#include <click/element.hh>
#include <click/ipaddress.hh>
#include <click/hashtable.hh>
#include <click/vector.hh>
#include "castor.hh"

CLICK_DECLS

class CastorXcastDestinationMap: public Element {
public:
	CastorXcastDestinationMap();
	~CastorXcastDestinationMap();
		
	const char *class_name() const	{ return "CastorXcastDestinationMap"; }
	const char *port_count() const	{ return PORTS_0_0; }
	const char *processing() const	{ return AGNOSTIC; }

	const Vector<IPAddress>& getDestinations(IPAddress multicastAddr) const;

    void add_handlers();
private:
	HashTable<IPAddress, Vector<IPAddress> > _map;
	Vector<IPAddress> _empty;

	int insertDestinations(IPAddress group, const Vector<IPAddress>&);

    static int write_handler(const String &, Element *, void *, ErrorHandler *);
};

CLICK_ENDDECLS

#endif
