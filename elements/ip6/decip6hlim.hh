#ifndef DECIP6HLIM_HH
#define DECIP6HLIM_HH

/*
 * =c
 * DecIP6HLIM()
 * =d
 * Expects IP6 packet as input.
 * If the hlim is <= 1 (i.e. has expired),
 * DecIP6HLIM sends the packet to output 1 (or discards it if there is no
 * output 1).
 * Otherwise it decrements the hlim,
 * and sends the packet to output 0.
 * 
 * Ordinarily output 1 is connected to an ICMP6 error packet generator.
 *
 * =e
 * This is a typical IP6 input processing sequence:
 *
 * = ... -> CheckIP6Header -> dt::DECIP6HLIM -> ...
 * = dt[1] -> ICMP6Error(18.26.4.24, 11, 0) -> ...
 *
 * =a ICMP6Error
 * =a CheckIP6Header
 */

#include "element.hh"
#include "glue.hh"

class DecIP6HLIM : public Element {

  int _drops;
  
 public:
  
  DecIP6HLIM();
  ~DecIP6HLIM();
  
  const char *class_name() const		{ return "DecIP6HLIM"; }
  const char *processing() const		{ return "a/ah"; }
  void notify_noutputs(int);
  
  int drops()					{ return _drops; }
  
  DecIP6HLIM *clone() const;
  void add_handlers();

  Packet *simple_action(Packet *);
  void drop_it(Packet *);

  //  inline Packet *smaction(Packet *);
//    void push(int, Packet *p);
//    Packet *pull(int); 
  
};

#endif
