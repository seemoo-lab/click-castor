#ifndef CLICK_CASTOR_XCAST_DESTINATION_MAP_HH
#define CLICK_CASTOR_XCAST_DESTINATION_MAP_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/vector.hh>
#include "../group_id.hh"
#include "../node_id.hh"

CLICK_DECLS

class XcastDestinationMap: public Element {
public:
	const char *class_name() const	{ return "XcastDestinationMap"; }
	const char *port_count() const	{ return PORTS_0_0; }
	const char *processing() const	{ return AGNOSTIC; }

	const Vector<NodeId>& get(const GroupId& group) const;

	void add_handlers();
private:
	HashTable<GroupId, Vector<NodeId> > map;
	const Vector<NodeId> empty;

	int insert(const GroupId& group, const Vector<NodeId>&);

	static int write_handler(const String&, Element*, void*, ErrorHandler*);
};

CLICK_ENDDECLS

#endif
