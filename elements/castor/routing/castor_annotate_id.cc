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
#include "castor_annotate_id.hh"
#include "../castor_anno.hh"

CLICK_DECLS

int CastorAnnotateId::configure(Vector<String> &conf, ErrorHandler *errh) {
	String _offset;
	if (Args(conf, this, errh)
			.read_mp("ID", id)
			.read_mp("OFFSET", AnyArg(), _offset)
			.complete() < 0)
		return -1;
	if (_offset == "SRC")
		offset = CastorAnno::src_id_anno_offset;
	else if (_offset == "DST")
		offset = CastorAnno::dst_id_anno_offset;
	else if (_offset == "HOP")
		offset = CastorAnno::hop_id_anno_offset;
	else {
		Args args(errh);
		args.push_back("OFFSET " + _offset);
		return args.read_mp("OFFSET", offset).complete();
	}
	return 0;
}

Packet* CastorAnnotateId::simple_action(Packet* p) {
	CastorAnno::id_anno(p, offset) = id;
	return p;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAnnotateId)
