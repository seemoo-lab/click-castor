#ifndef CLICK_CASTOR_CONTINUOUS_FLOW_MAP_HH
#define CLICK_CASTOR_CONTINUOUS_FLOW_MAP_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include "../castor.hh"

CLICK_DECLS

class CastorContinuousFlowMap : public Element {
public:
	const char *class_name() const { return "CastorContinuousFlowMap"; }
	const char *port_count() const { return PORTS_0_0; }
	const char *processing() const { return AGNOSTIC; }

	void update(const Hash &flow, const NodeId &src, const NodeId &dst);
	const Hash &previous(const NodeId &src, const NodeId &dst) const;

private:
	HashTable<Pair<NodeId, NodeId>, Hash> srcdstmap;
	HashTable<             NodeId , Hash>    dstmap;

	const Hash default_fid;
};

CLICK_ENDDECLS

#endif //CLICK_CASTOR_CONTINUOUS_FLOW_MAP_HH
