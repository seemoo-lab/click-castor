#ifndef CLICK_FLOODING_CHECK_DUPLICATE_HH
#define CLICK_FLOODING_CHECK_DUPLICATE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "flooding.hh"

CLICK_DECLS

class FloodingCheckDuplicate : public Element {
	public:
		const char *class_name() const	{ return "FloodingCheckDuplicate"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }

		void push(int, Packet *);

	private:
		typedef unsigned long Key;
		HashTable<Key, HashTable<Key,Key> > history;
};

CLICK_ENDDECLS

#endif
