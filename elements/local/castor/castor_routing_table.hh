#ifndef CLICK_CASTOR_ROUTING_TABLE_HH
#define CLICK_CASTOR_ROUTING_TABLE_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "castor.hh"

CLICK_DECLS

class CastorEstimator {
public:
	CastorEstimator(double updateDelta = 0.0) : updateDelta(updateDelta), alpha_all(0), beta_all(1), alpha_first(0), beta_first(1) { };
	double getEstimate() const {
		double s_all = alpha_all / (alpha_all + beta_all);
		double s_first = alpha_first / (alpha_first + beta_first);
		return (s_all + s_first) / 2;
	}
	void increaseFirst() {
		alpha_first = updateDelta * alpha_first + 1;
		beta_first = updateDelta * beta_first;
	}
	void increaseAll() {
		alpha_all = updateDelta * alpha_all + 1;
		beta_all = updateDelta * beta_all;
	}
	void decreaseFrist() {
		alpha_first = updateDelta * alpha_first;
		beta_first = updateDelta * beta_first + 1;
	}
	void decreaseAll() {
		alpha_all = updateDelta * alpha_all;
		beta_all = updateDelta * beta_all + 1;
	}
private:
	double updateDelta;
	double alpha_all;
	double beta_all;
	double alpha_first;
	double beta_first;
};

class CastorRoutingTable : public Element {
public:
	typedef NodeId SubflowId;

	CastorRoutingTable() : updateDelta(0.8) {}

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

	/**
	 * Adaptivity of the reliability estimators
	 */
	double updateDelta;

	void printRoutingTable(const FlowId&, SubflowId);

	template <typename K, typename V>
	V& getEntryInsertDefault(HashTable<K, V>& map, const K& key, const V& default_value = V());
};

CLICK_ENDDECLS

#endif
