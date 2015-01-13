#ifndef CLICK_CASTOR_ROUTINGTABLE_HH
#define CLICK_CASTOR_ROUTINGTABLE_HH

#include <click/element.hh>
#include <click/vector.hh>
#include "castor.hh"
#include "castor_neighbors.hh"

CLICK_DECLS

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
	 * Perform the Lookup operation, determines the best route for Packet with given fid and subfid
	 */
	IPAddress lookup(const FlowId& flow, IPAddress subflow);

	/**
	 * Updates the Estimation values in the Database for Flow Host
	 * Third parameter is true for successfull updading, else
	 */
	void updateEstimates(const FlowId& flow, IPAddress subflow, IPAddress neighbor, Operation, Estimate);

private:
	typedef struct RoutingEntry {
		IPAddress nextHop;
		double alpha_all;
		double beta_all;
		double alpha_first;
		double beta_first;
		RoutingEntry(IPAddress nextHop) : nextHop(nextHop), alpha_all(0), beta_all(1), alpha_first(0), beta_first(1) { };
	} RoutingEntry;

	class FlowEntry {
	public:
		FlowEntry(const FlowId& flow, IPAddress subflow) : subflow(subflow) {
			memcpy(this->flow, flow, sizeof(FlowId));
			routes = Vector<RoutingEntry>();
		}
		FlowId flow;
		IPAddress subflow;
		Vector<RoutingEntry> routes;
	};

	Vector<RoutingEntry>& getRoutingTable(const FlowId& flow, IPAddress subflow);
	RoutingEntry& getRoutingEntry(Vector<RoutingEntry>&, IPAddress);
	double getEstimate(const RoutingEntry&) const;
	void printRoutingTable(const FlowId&, IPAddress);

	Vector<FlowEntry> _flows;

	CastorNeighbors* neighbors;

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
