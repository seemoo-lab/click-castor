#include <click/config.h>
#include <click/confparse.hh>
#include "castor_next_flow_table.hh"

CLICK_DECLS

bool CastorNextFlowTable::set(const NextFlowAuth& nfauth, const FlowId& fid, NodeId subflow) {
#ifdef CASTOR_CONTINUOUS_FLOW
	Entry* entry = map.get_pointer(nfauth);
	if (entry == 0) {
		Entry newEntry;
		newEntry.fid = fid;
		newEntry.subflows.set(subflow, subflow);
		map.set(nfauth, newEntry);
		return true;
	} else if (entry->fid != fid) {
		click_chatter("[CastorNextFlowTable] Warning: trying to overwrite next flow mapping (should not happen)");
		return false;
	} else if (entry->subflows.count(subflow) == 0) {
		entry->subflows.set(subflow, subflow);
		return true;
	} else {
		return false;
	}
#else
	(void) nfauth; (void) fid; (void) subflow;
	return false;
#endif
}

void CastorNextFlowTable::remove(const NextFlowAuth& nfauth, NodeId subflow) {
	Entry* entry = map.get_pointer(nfauth);
	if (!entry)
		return;
	entry->subflows.erase(subflow);
	if (entry->subflows.size() == 0)
		// last element
		map.erase(nfauth);
}

const FlowId* CastorNextFlowTable::get(const NextFlowAuth& nfauth, NodeId subflow) const {
	const Entry* entry = map.get_pointer(nfauth);
	if (!entry || entry->subflows.count(subflow) == 0)
		return NULL;
	return &entry->fid;
}

void CastorNextFlowTable::print(const NextFlowAuth& nfauth) {
	click_chatter("Entry for %s", nfauth.str().c_str());
	const Entry* entry = map.get_pointer(nfauth);
	if (!entry) {
		click_chatter("  empty");
	} else {
		click_chatter("  %s", entry->fid.str().c_str());
		for (HashTable<NodeId, NodeId>::const_iterator it = entry->subflows.begin(); it != entry->subflows.end(); it++)
			click_chatter("      %s", it->first.unparse().c_str());
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorNextFlowTable)
