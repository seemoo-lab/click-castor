#ifndef CLICK_CASTOR_XCAST_DESTINATION_MAP_HH
#define CLICK_CASTOR_XCAST_DESTINATION_MAP_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/vector.hh>
#include "castor.hh"
#include "castor_xcast.hh"

CLICK_DECLS

class CastorXcastDestinationMap: public Element {
public:
	const char *class_name() const	{ return "CastorXcastDestinationMap"; }
	const char *port_count() const	{ return PORTS_0_0; }
	const char *processing() const	{ return AGNOSTIC; }

	const Vector<NodeId>& getDestinations(GroupId group) const;

    void add_handlers();
private:
	HashTable<GroupId, Vector<NodeId> > _map;
	Vector<NodeId> _empty;

	int insertDestinations(GroupId group, const Vector<NodeId>&);

    static int write_handler(const String &, Element *, void *, ErrorHandler *);
};

CLICK_ENDDECLS

#endif
