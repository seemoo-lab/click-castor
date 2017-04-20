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
#include <click/args.hh>
#include "castor_add_pkt_tohistory.hh"
#include "../castor.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorAddPktToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_mp("CastorHistory", ElementCastArg("CastorHistory"), history)
			.complete();
}

Packet* CastorAddPktToHistory::simple_action(Packet* p){
	const CastorPkt& pkt = *reinterpret_cast<const CastorPkt*>(p->data());

	history->addPkt(pkt.pid, ntohs(pkt.kpkt), pkt.fid, CastorAnno::src_id_anno(p), CastorAnno::dst_id_anno(p), pkt.src, pkt.dst, p->timestamp_anno());

	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddPktToHistory)
