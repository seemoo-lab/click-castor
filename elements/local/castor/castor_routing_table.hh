#ifndef CLICK_CASTOR_ROUTING_TABLE_HH
#define CLICK_CASTOR_ROUTING_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "castor.hh"

CLICK_DECLS

class ExponentialMovingAverage {
public:
	ExponentialMovingAverage (double delta) : delta(delta), alpha(0), beta(1) { };
	double get() const {
		return alpha / (alpha + beta);
	}
	void increase() {
		alpha = delta * alpha + 1;
		beta  = delta * beta;
	}
	void decrease() {
		alpha = delta * alpha;
		beta  = delta * beta  + 1;
	}
private:
	double delta; // update weight
	double alpha; // running average of packets delivered
	double beta;  // running average of packets not delivered
};

class CastorEstimator {
public:
	CastorEstimator(double delta) : first(ExponentialMovingAverage(delta)), all(ExponentialMovingAverage(delta)) { };
	double getEstimate() const {
		return (all.get() + first.get()) / 2;
	}
	void increaseFirst() {
		first.increase();
	}
	void increaseAll() {
		all.increase();
	}
	void decreaseFrist() {
		first.decrease();
	}
	void decreaseAll() {
		all.decrease();
	}
private:
	ExponentialMovingAverage first;
	ExponentialMovingAverage all;
};

class CastorRoutingTable : public Element {
public:
	typedef NodeId SubflowId;

	CastorRoutingTable() : flows(FlowEntry(SubflowEntry(ForwarderEntry(CastorEstimator(0.0))))) { };

	const char *class_name() const { return "CastorRoutingTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	HashTable<NodeId, CastorEstimator>& getFlowEntry(const FlowId& flow, const SubflowId& subflow);

	CastorEstimator& getEstimator(const FlowId& flow, SubflowId subflow, NodeId forwarder);

	/**
	 * Copies the forwarder entry of one flow to another.
	 * If an entry for (newFlow, subflow) already exists, that entry is not overwritten.
	 * Returns true if entry was copied, false otherwise.
	 */
	bool copyFlowEntry(const FlowId& newFlow, const FlowId& oldFlow, NodeId subflow);
private:
	typedef HashTable<NodeId, CastorEstimator> ForwarderEntry;
	typedef HashTable<SubflowId, ForwarderEntry> SubflowEntry;
	typedef HashTable<FlowId, SubflowEntry> FlowEntry;

	FlowEntry flows;

	void printRoutingTable(const FlowId&, SubflowId);
};

CLICK_ENDDECLS

#endif
