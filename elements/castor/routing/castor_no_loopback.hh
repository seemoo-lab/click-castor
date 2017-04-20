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

#ifndef CLICK_CASTOR_NO_LOOPBACK_HH
#define CLICK_CASTOR_NO_LOOPBACK_HH

#include <click/element.hh>
#include "../castor.hh"
#include "castor_history.hh"

CLICK_DECLS

/**
 * Redirects an ACK that has arrived at the PKT source to output port 1, otherwise forwards it to port 0.
 */
class CastorNoLoopback: public Element {
public:
	const char *class_name() const { return "CastorNoLoopback"; }
	const char *port_count() const { return "1/2"; }
	const char *processing() const { return PUSH; }

	int configure(Vector<String>&, ErrorHandler*);

	void push(int, Packet *);

private:
	CastorHistory *history;
	NeighborId id;
};

CLICK_ENDDECLS

#endif
