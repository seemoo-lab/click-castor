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

#ifndef CLICK_SECURITY_ASSOCIATION_HH
#define CLICK_SECURITY_ASSOCIATION_HH

#include <click/element.hh>
#include <click/vector.hh>

CLICK_DECLS

class SecurityAssociation {
public:
	enum Type {
		pubkey, privkey, endofhashchain, sharedsecret
	};

	const Type type;
	Vector<uint8_t> secret;

	SecurityAssociation(Type type, const Vector<uint8_t>& secret) : type(type), secret(secret) { }
	String str() const;

private:
	String typeToStr(Type type) const;
};

CLICK_ENDDECLS

#endif
