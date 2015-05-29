#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "castor_timeout_table.hh"

CLICK_DECLS

int CastorTimeoutTable::configure(Vector<String> &conf, ErrorHandler *errh) {
	int ret = cp_va_kparse(conf, this, errh,
			"INIT", cpkP, cpUnsigned, &CastorTimeout::init_timeout,
			"MIN", cpkP, cpUnsigned, &CastorTimeout::min_timeout,
			"MAX", cpkP, cpUnsigned, &CastorTimeout::max_timeout,
			"ALPHA", cpkN, cpDouble, &CastorTimeout::alpha,
			"BETA", cpkN, cpDouble, &CastorTimeout::beta,
			cpEnd);
	if (CastorTimeout::init_timeout < CastorTimeout::min_timeout || CastorTimeout::init_timeout > CastorTimeout::max_timeout) {
		errh->fatal("INIT needs to between MIN and MAX");
		return -1;
	}
	if (CastorTimeout::alpha < 0 || CastorTimeout::alpha > 1 || CastorTimeout::beta < 0 || CastorTimeout::beta > 1) {
		errh->fatal("ALPHA and BETA need to be within 0 and 1");
		return -1;
	}
	return ret;
}

CastorTimeout& CastorTimeoutTable::getTimeout(const FlowId& flow, const SubflowId& subflow, NodeId forwarder) {
	CastorTimeout& entry = getEntryInsertDefault(getEntryInsertDefault(getEntryInsertDefault(flows, flow), subflow), forwarder);
	return entry;
}

template <typename K, typename V>
V& CastorTimeoutTable::getEntryInsertDefault(HashTable<K, V>& map, const K& key) {
	V* value = map.get_pointer(key);
	if(value == 0) {
		map.set(key, V()); // Insert default value
		value = map.get_pointer(key);
		assert(value != 0);
	}
	return *value;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorTimeoutTable)
