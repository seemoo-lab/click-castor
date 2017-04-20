/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

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
