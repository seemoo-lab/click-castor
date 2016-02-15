#ifndef CLICK_CASTOR_ADD_HEADER_HH
#define CLICK_CASTOR_ADD_HEADER_HH

#include <click/element.hh>
#include "castor.hh"
#include "flow/castor_flow_manager.hh"

CLICK_DECLS

/**
 * Add Castor header to IP packet
 * TODO: Should replace IP header instead of add
 */
class CastorAddHeader: public Element {
public:
	const char *class_name() const { return "CastorAddHeader"; }
	const char *port_count() const { return PORTS_1_1; }
	const char *processing() const { return AGNOSTIC; }
	int configure(Vector<String>&, ErrorHandler*);

	Packet* simple_action(Packet*);
private:
	CastorFlowManager* flow;

	inline unsigned int fauth_size(unsigned int k, unsigned int max) {
		unsigned int c;  // output: c will count k's trailing zero bits,
		        		 // so if k is 1101000 (base 2), then k will be 3
		if (k) {
			k = (k ^ (k - 1)) >> 1;  // Set k's trailing 0s to 1s and zero rest
			for (c = 0; k; c++)
				k >>= 1;
		} else {
			c = max;
		}
		return c;
	}
};

CLICK_ENDDECLS

#endif
