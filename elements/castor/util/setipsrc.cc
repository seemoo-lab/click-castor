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
#include <click/confparse.hh>
#include <click/packet.hh>
#include <clicknet/ip.h>
#include <click/packet_anno.hh>
#include "setipsrc.hh"

CLICK_DECLS

int SetIPSrc::configure(Vector<String>& conf, ErrorHandler* errh) {
	if (Args(conf, this, errh)
		.read_mp("ADDR", myAddr)
		.complete() < 0)
			return -1;
	return 0;
}

void SetIPSrc::push(int, Packet* p) {
	WritablePacket* q = p->uniqueify();

	click_ip *ip = q->ip_header();
	SET_FIX_IP_SRC_ANNO(p, 0);

#if 0
	click_chatter("SetIPSrc changed %x to %x",
			ip->ip_src.s_addr,
			myAddr.s_addr);
#endif

	ip->ip_src = myAddr;
	int hlen = ip->ip_hl << 2;
	ip->ip_sum = 0;
	ip->ip_sum = click_in_cksum((unsigned char *) ip, hlen);

	output(0).push(q);

}

CLICK_ENDDECLS
EXPORT_ELEMENT(SetIPSrc)
