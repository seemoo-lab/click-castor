/*
 * icmp6error.{cc,hh} -- element constructs ICMP6 error packets
 * Robert Morris
 *
 * Copyright (c) 1999-2000 Massachusetts Institute of Technology.
 *
 * This software is being provided by the copyright holders under the GNU
 * General Public License, either version 2 or, at your discretion, any later
 * version. For more information, see the `COPYRIGHT' file in the source
 * distribution.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "click_icmp6.h"
#include "click_ip6.h"
#include "click_ip.h"
#include "icmp6error.hh"
#include "ip6address.hh"
#include "confparse.hh"
#include "error.hh"
#include "glue.hh"

ICMP6Error::ICMP6Error()
{
  add_input();
  add_output();
  _code = _type = -1;
}

ICMP6Error::~ICMP6Error()
{
}

int
ICMP6Error::configure(const Vector<String> &conf, ErrorHandler *errh)
{
  if (cp_va_parse(conf, this, errh,
                  cpIP6Address, "Source IP address", &_src_ip,
                  cpInteger, "ICMP6 Type", &_type,
                  cpInteger, "ICMP6 Code", &_code,
		  0) < 0)
    return -1;
  return 0;
}

bool
ICMP6Error::is_error_type(int type)
{
  return(type == ICMP6_DST_UNREACHABLE ||  	// 1
         type == ICMP6_PKT_TOOBIG ||		// 2
         type == ICMP6_TYPE_TIME_EXCEEDED ||	// 3
	 type == ICMP6_PARAMETER_PROBLEM);       // 4

}

int
ICMP6Error::initialize(ErrorHandler *errh)
{
  
  if (_type < 0 || _code < 0 || _src_ip.addr().isZero())
    return errh->error("not configured");
  if(is_error_type(_type) == false)
    return errh->error("ICMP6 type %d is not an error type", _type);
  return 0;
}

/*
 * Is an IP6 address unicast?
 * Can't detect directed broadcast here!
 */
/*
bool
ICMP6Error::unicast(struct in6my_addr aa)
{
  unsigned int a = aa.s_addr;
  unsigned int ha = ntohl(a);
*/
  /* limited broadcast */
/*  
if(ha == 0xffffffff)
    return(0);
  */

  /* Class D multicast */
/*  
 if((ha & 0xf0000000u) == 0xe0000000u)
    return(0);

  return(1);
}
*/

/*
 * Is a source IP address valid as defined in RFC1812 5.3.7
 * or 4.2.2.11 or 4.2.3.1?
 */
/*
bool
ICMP6Error::valid_source(struct in_addr aa)
{
  unsigned int a = aa.s_addr;
  unsigned int ha = ntohl(a);
  unsigned net = (ha >> 24) & 0xff;
*/
  /* broadcast or multicast */
// if(unicast(aa) == 0)
//    return(0);

  /* local net or host: */
// if(net == 0)
//    return(0);

  /* I don't know how to detect directed broadcast. */

  /* 127.0.0.1 */
//  if(net == 127)
//    return(0);

  /* Class E */
//  if((net & 0xf0) == 0xf0)
//    return(0);

//  return(1);
//}

/*
 * Does a packet contain a source route option?
 */
/*
bool
ICMP6Error::has_route_opt(const click_ip6 *ip)
{
  int opts = (ip->ip_hl << 2) - sizeof(click_ip6);
  u_char *base = (u_char *) (ip + 1);
  int i, optlen;

  for(i = 0; i < opts; i += optlen){
    int opt = base[i];
    if(opt == IPOPT_LSRR || opt == IPOPT_SSRR)
      return(1);
    if(opt == IPOPT_EOL)
      break;
    if(opt == IPOPT_NOP){
      optlen = 1;
    } else {
      optlen = base[i+1];
    }
  }

  return(0);
}

*/

Packet *
ICMP6Error::simple_action(Packet *p)
{
  WritablePacket *q = 0;
  click_ip6 *ipp = p->ip6_header();
  click_ip6 *nip;
  struct icmp6_generic *icp;
  // unsigned hlen, xlen;
  unsigned xlen;
  static int id = 1;

  if (!ipp)
    goto out;

  //hlen = ipp->ip_hl * 4;

  /* These "don'ts" are from RFC1885 2.4.e: */

  /* Don't reply to ICMP6 error messages. */
  if(ipp->ip6_nxt == IP_PROTO_ICMP) {
    icp = (struct icmp6_generic *) ((char *)ipp);
    if( is_error_type(icp->icmp6_type))
      goto out;
  }

  /* Don't respond to packets with IPv6 broadcast destinations. */
  //if(unicast(ipp->ip6_dst) == 0)
  //  goto out;

  /* Don't respond to e.g. Ethernet broadcasts or multicasts. */
  //if(p->mac_broadcast_anno())
  //  goto out;

  //not sure about the following three
  /* Don't respond is src is net 0 or invalid. */
  // if(valid_source(ipp->ip6_src) == 0)
  //  goto out;

  /* Don't respond to fragments other than the first. */
  /*
    if(ntohs(ipp->ip_off) & IP_OFFMASK){
    goto out;
  }
  */

  /* Don't send a redirect for a source-routed packet. 5.2.7.2 */
  /*
    if(_type == 5 && has_route_opt(ipp))
    goto out;
  */

  /* send back as much of invoding packet as will fit without the ICMPv6 packet exceeding 576 octets , ICMP header is 8 octets*/

  xlen = p->length();
  if (xlen > 568)
    xlen = 568;           

  q = Packet::make(sizeof(struct icmp6_generic) + xlen);
  // guaranteed that packet data is aligned
  memset(q->data(), '\0', q->length());
  nip = (click_ip6 *) q->data();
  //nip->ip_v = IPVERSION;
  nip->ip6_v = 6;
  //nip->ip_hl = sizeof(click_ip) >> 2;
  //nip->ip_len = htons(q->length());
  nip->ip6_plen = htons(q->length());
  nip->ip6_nxt = IP_PROTO_ICMP;  /* next header */
  //nip->ip_id = htons(id++);
  //nip->ip_p = IP_PROTO_ICMP; /* icmp */
  //nip->ip_ttl = 200;
  nip->ip6_hlim = 200;
  //nip->ip6_hlim = 2;
  nip->ip6_src = IP6Address(_src_ip.addr());
  nip->ip6_dst = ipp->ip6_src;
  //nip->ip6_src = new IP6Address(_src_ip.in_addr());
  //nip->ip6_dst = new IP6Address(ipp->ip6_src);

  //nip->ip_sum = in_cksum((unsigned char *) nip, sizeof(click_ip));

 
  icp = (struct icmp6_generic *) (nip + 1);
  icp->icmp6_type = _type;
  icp->icmp6_code = _code;

    
if(_type == 2 && _code == 0){
    /* Set the mtu value. */
  //icp = (struct icmp6_pkt_toobig *) (nip + 1);
     //icp->icmp6_type = _type;
     //icp->icmp6_code = _code;
  ((struct icmp6_pkt_toobig *)icp)->icmp6_mtusize = 1500;
  //icp->icmp6_mtusize = 1500;
  }
  
  if(_type == 4 && _code == 0){
    /* Set the Parameter Problem pointer. */
    ((struct icmp6_param *) icp)->pointer[1] = p->param_off_anno();
  }
  
  memcpy((void *)(icp + 1), p->data(), xlen-40);
  icp->icmp6_cksum = in_cksum((unsigned char *)icp, sizeof(icmp6_generic) + xlen);

  q->set_dst_ip6_anno(IP6Address(nip->ip6_dst));
  q->set_fix_ip_src_anno(1);
  q->set_ip6_header(nip, sizeof(click_ip6));

 out:
  p->kill();
  return(q);
}

EXPORT_ELEMENT(ICMP6Error)
