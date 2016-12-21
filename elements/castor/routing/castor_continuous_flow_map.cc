#include <click/config.h>
#include "castor_continuous_flow_map.hh"

CLICK_DECLS

void CastorContinuousFlowMap::update(const Hash &flow, const NodeId &src, const NodeId &dst) {
	Pair<NodeId,NodeId> pair(src, dst);
	srcdstmap[pair] = flow;
	dstmap[dst] = flow;
}

const Hash &CastorContinuousFlowMap::previous(const NodeId &src, const NodeId &dst) const {
	Pair<NodeId, NodeId> pair(src, dst);

	if (srcdstmap.count(pair) > 0) {
		return srcdstmap[pair];
	} else if (dstmap.count(dst) > 0){
		return dstmap[dst];
	} else {
		return default_fid;
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorContinuousFlowMap)
