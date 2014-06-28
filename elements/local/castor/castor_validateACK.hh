/*
 * castor_validateACK.hh
 *
 *  Created on: Jun 23, 2014
 *      Author: milan
 */

#ifndef CLICK_CASTORVALIDATEACK_HH
#define CLICK_CASTORVALIDATEACK_HH

#include <click/element.hh>
#include "castor.hh"
#include "castor_history.hh"

CLICK_DECLS

class CastorValidateACK : public Element {
	public:
		CastorValidateACK();
		~CastorValidateACK();

		const char *class_name() const	{ return "CastorValidateACK"; }
		const char *port_count() const	{ return "1/2"; }
		const char *processing() const	{ return PUSH; }
		int configure(Vector<String>&, ErrorHandler*);

		void push(int, Packet *);
	private:
		CastorHistory* 		_history;
};

CLICK_ENDDECLS

#endif /* CASTOR_VALIDATEACK_HH_ */
