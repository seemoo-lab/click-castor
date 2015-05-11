#ifndef CLICK_SETIPSRC_HH
#define CLICK_SETIPSRC_HH

#include <click/element.hh>
#include <click/ipaddress.hh>

CLICK_DECLS

/**
 * Sets the source IP address of the packet to the specified address
 */
class SetIPSrc: public Element {
public:
	const char *class_name() const { return "SetIPSrc"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return PUSH; }
	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	IPAddress myAddr;
};

CLICK_ENDDECLS

#endif
