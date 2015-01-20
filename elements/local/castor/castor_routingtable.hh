#ifndef CLICK_CASTOR_ROUTINGTABLE_HH
#define CLICK_CASTOR_ROUTINGTABLE_HH

#include <click/element.hh>
#include <click/vector.hh>
#include <click/hashtable.hh>
#include "castor.hh"

CLICK_DECLS

class CastorRoutingTable : public Element {
public:
	typedef IPAddress SubflowId;

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

	class ForwarderEntry {
	public:
		ForwarderEntry() { }
		size_t size() const { return estimators.size(); }
		void clear() { estimators.clear(); };
		CastorEstimator& getEstimator(IPAddress neighbor) {
			CastorEstimator* entry = 0;
			if(!(entry = estimators.get_pointer(neighbor))) {
				estimators.set(neighbor, CastorEstimator());
				entry = estimators.get_pointer(neighbor);
			}
			assert(entry);
			return *entry;
		}
		HashTable<IPAddress, CastorEstimator> estimators;
	};

	class FlowEntry {
	public:
		FlowEntry() { }
		ForwarderEntry& getForwarderEntry(SubflowId subflow) {
			ForwarderEntry* entry = 0;
			if(!(entry = subflows.get_pointer(subflow))) {
				subflows.set(subflow, ForwarderEntry());
				entry = subflows.get_pointer(subflow);
			}
			assert(entry);
			return *entry;
		}
		HashTable<SubflowId, ForwarderEntry> subflows;
	};

	CastorRoutingTable();

	enum Operation { increase, decrease };
	enum Estimate { first, all };

	const char *class_name() const { return "CastorRoutingTable"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	FlowEntry& getFlowEntry(const FlowId& flow);

	void updateEstimates(const FlowId& flow, SubflowId subflow, IPAddress forwarder, Operation, Estimate);

private:
	void printRoutingTable(const FlowId&, IPAddress);

	HashTable<FlowId, FlowEntry> flows;
	/**
	 * Adaptivity of the reliability estimators
	 */
	double updateDelta;
};

CLICK_ENDDECLS

#endif
