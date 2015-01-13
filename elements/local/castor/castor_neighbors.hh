#ifndef CLICK_CASTOR_NEIGHBORS_HH
#define CLICK_CASTOR_NEIGHBORS_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "castor.hh"
#include <click/timestamp.hh>

CLICK_DECLS

typedef IPAddress Neighbor;

class CastorNeighbors: public Element {
public:
	CastorNeighbors();
	~CastorNeighbors();

	const char *class_name() const { return "CastorNeighbors"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	/**
	 * Inserts the neighbor. If an entry already exists, it is updated with the current timestamp.
	 */
	void addNeighbor(Neighbor neighbor);

	/**
	 * Whether such a neighbor exists and the entry has not yet timed out.
	 */
	bool hasNeighbor(Neighbor neighbor);

private:
	/* TODO Map is never cleared, maybe use Timer to do regular cleanup */
	HashTable<Neighbor, Timestamp> neighbors;
	unsigned int timeout;

	Timestamp* getEntry(Neighbor neighbor);
};

CLICK_ENDDECLS

#endif
