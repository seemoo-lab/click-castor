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
#include "securityassociation.hh"

CLICK_DECLS

String SecurityAssociation::typeToStr(Type type) const {
	switch (type) {
	case privkey: return String("privkey");
	case pubkey: return String("pubkey");
	case endofhashchain: return String("endofhashchain");
	case sharedsecret: return String("sharedsecret");
	default: return String("unknown");
	}
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(SecurityAssociation);
