#ifndef CLICK_CASTOR_ROUTINGTABLE_HH
#define CLICK_CASTOR_ROUTINGTABLE_HH

#include <click/element.hh>
#include <click/vector.hh>
#include "castor.hh"

CLICK_DECLS

typedef struct RoutingEntry {
	IPAddress nextHop;
	double alpha_all;
	double beta_all;
	double alpha_first;
	double beta_first;
	RoutingEntry(IPAddress nextHop) : nextHop(nextHop), alpha_all(0), beta_all(1), alpha_first(0), beta_first(1) { };
} RoutingEntry;

typedef struct {
	FlowId flow;
	Vector<RoutingEntry> routes;
} FlowEntry;

class CastorRoutingTable: public Element {
public:
	CastorRoutingTable();
	~CastorRoutingTable();

	enum Operation { increase, decrease };
	enum Estimate { first, all };

	const char *class_name() const { return "CastorRoutingTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	/**
	 * Perform the Lookup operation, determines the best route for Packet with given FlowID
	 */
	IPAddress lookup(const FlowId&);

	/**
	 * Updates the Estimation values in the Database for Flow Host
	 * Third parameter is true for successfull updading, else
	 */
	void updateEstimates(const FlowId&, const IPAddress&, Operation, Estimate);

private:
	Vector<RoutingEntry>& getRoutingTable(const FlowId& flow);
	RoutingEntry& getRoutingEntry(Vector<RoutingEntry>&, const IPAddress&);
	double getEstimate(const RoutingEntry&) const;
	void printRoutingTable(const FlowId&);

	Vector<FlowEntry> _flows;
	/**
	 * Bandwidth investment for route discovery (larger values reduce the broadcast probability)
	 */
	double broadcastAdjust;
	/**
	 * Adaptivity of the reliability estimators
	 */
	double updateDelta;

};

CLICK_ENDDECLS

#endif
