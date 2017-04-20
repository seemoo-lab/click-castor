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
#include <click/error.hh>
#include "crypto.hh"

CLICK_DECLS

int Crypto::configure(Vector<String> &conf, ErrorHandler *errh) {
	return Args(conf, this, errh)
			.read_p("SAM", ElementCastArg("SAManagement"), sam)
			.complete();
}

int Crypto::initialize(ErrorHandler* errh) {
	if (sodium_init() == -1) {
		errh->fatal("libsodium could not be initialized");
		return -1;
	} else {
		return 0;
	}
}

void Crypto::random(uint8_t* buf, unsigned int length) const {
	randombytes_buf(buf, length);
}

/**
 * Return the symmetric shared key for a destination.
 */
const SymmetricKey* Crypto::getSharedKey(const NodeId& id) const {
	if (!sam)
		return 0;
	const SecurityAssociation* sharedKeySA = sam->get(id, SecurityAssociation::sharedsecret);
	if (!sharedKeySA)
		return 0;
	return &sharedKeySA->secret;
}
const SymmetricKey* Crypto::getSharedKey(const NeighborId& id) const {
	if (!sam)
		return 0;
	const SecurityAssociation* sharedKeySA = sam->get(id, SecurityAssociation::sharedsecret);
	if (!sharedKeySA)
		return 0;
	return &sharedKeySA->secret;
}

void Crypto::hash(uint8_t* out, unsigned int outlen, const uint8_t* in, unsigned int inlen) const {
	if (outlen < crypto_generichash_BYTES_MIN || outlen > crypto_generichash_BYTES_MAX) {
		click_chatter("Cannot produce hash values of size %u", outlen);
		return;
	}
	crypto_generichash(out, outlen, in, inlen, NULL, 0);
}

void Crypto::auth(uint8_t* out, const uint8_t* in, unsigned inlen, const uint8_t* key) {
	crypto_shorthash(out, in, inlen, key);
}

void Crypto::auth(Buffer<crypto_shorthash_BYTES>& out, const uint8_t* in, unsigned int inlen, const uint8_t* key) {
	// FIXME: key should be derived from 'master shared secret'
	crypto_shorthash(out.data(), in, inlen, key);
}

CLICK_ENDDECLS

ELEMENT_LIBS(-lsodium)
ELEMENT_REQUIRES(userlevel)
EXPORT_ELEMENT(Crypto)
