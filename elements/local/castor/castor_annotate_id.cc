#include <click/config.h>
#include <click/args.hh>
#include "castor_annotate_id.hh"
#include "castor_anno.hh"

CLICK_DECLS

int CastorAnnotateId::configure(Vector<String> &conf, ErrorHandler *errh) {
	String _offset;
	if (Args(conf, errh)
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
