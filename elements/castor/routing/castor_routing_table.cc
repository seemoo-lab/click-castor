#include <click/config.h>
#include <click/args.hh>
#include <click/straccum.hh>
#include <click/error.hh>
#include "castor_routing_table.hh"

#ifndef MAX
  #define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

CLICK_DECLS

int CastorRoutingTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (Args(conf, this, errh)
			.read_mp("UpdateDelta", updateDelta)
			.read_or_set("TIMEOUT", timeout, 10000)
			.read_or_set("CLEAN", clean_interval, 1000)
			.complete() < 0)
		return -1;
	if (updateDelta < 0 || updateDelta > 1) {
		errh->error("Invalid updateDelta value: %f (should be between 0 and 1)", updateDelta);
		return -1;
	}
	// Gratuitous user warnings if values seem 'impractical'
	if (updateDelta < 0.30)
		errh->warning("Possibly unwanted updateDelta value: %f (reliability estimator adaption is too fast)", updateDelta);
	if (updateDelta > 0.95)
		errh->warning("Possibly unwanted updateDelta value: %f (reliability estimator adaption is very slow)", updateDelta);

	timer.initialize(this);
	default_entry = FlowEntry(CastorEstimator(updateDelta));

	return 0;
}

CastorRoutingTable::size_type CastorRoutingTable::count(const Hash &flow) const {
	return flows.count(flow);
}

CastorRoutingTable::size_type CastorRoutingTable::size() const {
	return flows.size();
}

CastorRoutingTable::FlowEntry& CastorRoutingTable::at_or_default(const Hash &flow, const FlowEntry &init) {
	// TODO: only allocate flow state after first ACK received
	Timestamp node_timeout = Timestamp::recent_steady() + Timestamp::make_msec(timeout);

	ListNode **node_ptr = flows.get_pointer(flow);
	ListNode *node = NULL;
	if (node_ptr == NULL) {
		node = new ListNode(flow, init, node_timeout);
		flows.set(flow, node);
		bool empty = timeout_queue.empty();
		timeout_queue.push_back(node);
		// Start the timer if it has been inactive
		if (empty)
			timer.schedule_at_steady(node_timeout);
	} else {
		node = *node_ptr;
		bool isFront = node == timeout_queue.front();
		timeout_queue.erase(node);
		node->timeout = node_timeout;
		timeout_queue.push_back(node);
		if (isFront) {
			node = timeout_queue.front();
			timer.unschedule();
			timer.schedule_at_steady(node->timeout);
		}
	}
	return node->entry;
}

void CastorRoutingTable::insert(const Hash &flow, const CastorRoutingTable::FlowEntry &entry) {
	(void) at_or_default(flow, entry);
}

String CastorRoutingTable::unparse(const Hash& flow) const {
	StringAccum sa;
	sa << "Flow " << flow.str() << ":\n";
	const auto& fe = flows[flow]->entry;
	if(fe.size() == 0)
		sa << " - EMPTY \n";
	else
		for (const auto&  it : fe)
			sa << " - " << it.first << "\t" << it.second.getEstimate() << "\n";
	return String(sa.c_str());
}

void CastorRoutingTable::run_timer(Timer* _timer) {
	assert(_timer == &timer);

	ListNode *node;

	unsigned int removed = 0;
	// Fetch expired route entries from the timer queue and remove them from the routing table
	for (;; ++removed) {
		node = timeout_queue.front();
		if (node == NULL)
			break;
		if (Timestamp::recent_steady() - node->timeout < 0)
			break;
		timeout_queue.pop_front();
		flows.erase(node->id);
		delete node;
	}

	click_chatter("RT CLEAN: delete %u, left %u", removed, flows.size());

	if (node != NULL) {
		// Restart the timer with the timeout of the first expiring neighbor
		Timestamp next_clean_time = MAX(node->timeout, Timestamp::recent_steady() + Timestamp::make_msec(clean_interval));
		timer.schedule_at_steady(next_clean_time);
	}
}

void CastorRoutingTable::print(const Hash& flow) const {
	click_chatter(unparse(flow).c_str());
}

void CastorRoutingTable::add_handlers() {
	add_read_handler("print", read_table_handler, 0);
}

String CastorRoutingTable::read_table_handler(Element *e, void *) {
	CastorRoutingTable* rt = (CastorRoutingTable*) e;
	StringAccum sa;
	sa << "Total number of entries: " << rt->flows.size() << "\n";
	for (const auto& fe : rt->flows)
		sa << rt->unparse(fe.first);
	return String(sa.c_str());
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRoutingTable)
