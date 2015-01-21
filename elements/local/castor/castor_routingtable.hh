#ifndef CLICK_CASTOR_ROUTINGTABLE_HH
#define CLICK_CASTOR_ROUTINGTABLE_HH

#include <click/element.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
#include "castor.hh"

CLICK_DECLS

class CastorEstimator {
public:
	CastorEstimator() : alpha_all(0), beta_all(1), alpha_first(0), beta_first(1) { };
	double getEstimate() const {
		double s_all = alpha_all / (alpha_all + beta_all);
		double s_first = alpha_first / (alpha_first + beta_first);
		return (s_all + s_first) / 2;
	}
	void increaseFirst(double updateDelta) {
		alpha_first = updateDelta * alpha_first + 1;
		beta_first = updateDelta * beta_first;
	}
	void increaseAll(double updateDelta) {
		alpha_all = updateDelta * alpha_all + 1;
		beta_all = updateDelta * beta_all;
	}
	void decreaseFrist(double updateDelta) {
		alpha_first = updateDelta * alpha_first;
		beta_first = updateDelta * beta_first + 1;
	}
	void decreaseAll(double updateDelta) {
		alpha_all = updateDelta * alpha_all;
		beta_all = updateDelta * beta_all + 1;
	}
private:
	double alpha_all;
	double beta_all;
	double alpha_first;
	double beta_first;
};

class CastorRoutingTable : public Element {
public:
	typedef IPAddress SubflowId;

	CastorRoutingTable();

	enum Operation { increase, decrease };
	enum Estimate { first, all };

	const char *class_name() const { return "CastorRoutingTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	HashTable<IPAddress, CastorEstimator>& getFlowEntry(const FlowId& flow, const SubflowId& subflow);

	void updateEstimates(const FlowId& flow, SubflowId subflow, IPAddress forwarder, Operation, Estimate);

private:
	typedef HashTable<IPAddress, CastorEstimator> ForwarderEntry;
	typedef HashTable<SubflowId, ForwarderEntry> SubflowEntry;
	typedef HashTable<FlowId, SubflowEntry> FlowEntry;
	FlowEntry flows;
	/**
	 * Adaptivity of the reliability estimators
	 */
	double updateDelta;

	void printRoutingTable(const FlowId&, SubflowId);

	template <typename K, typename V>
	static V& getEntryInsertDefault(HashTable<K, V>& map, const K& key);
};

CLICK_ENDDECLS

#endif
