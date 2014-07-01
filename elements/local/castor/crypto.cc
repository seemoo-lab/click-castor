#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include <click/straccum.hh>
#include "crypto.hh"

#include <botan/hash.h>
#include <botan/sha160.h>
#include <botan/auto_rng.h>
#include <botan/rsa.h>
#include <botan/pubkey.h>
#include <botan/symkey.h>
#include <botan/aes.h>

CLICK_DECLS

Crypto::Crypto() {
}

Crypto::~Crypto() {
}

/*
 * Configure the Crypto Element
 */
int Crypto::configure(Vector<String> &conf, ErrorHandler *errh)
{
	int res = cp_va_kparse(conf, this, errh,
		"SAM", cpkP+cpkM, cpElementCast, "SAManagement", &_sam,
		cpEnd);
	if(res < 0) return res;

	blockCipher = new Botan::AES_128();

	return 0;
}

void Crypto::hash(Hash hash, uint8_t* data, uint8_t length) {
	Botan::SHA_160 sha160;
	Botan::SecureVector<Botan::byte> hashed = sha160.process(data, length);
	memcpy(hash, hashed.begin(), CASTOR_HASHLENGTH);
}

void Crypto::randomize(Hash r) {
	Botan::AutoSeeded_RNG rng;
	rng.randomize((uint8_t*) r, CASTOR_HASHLENGTH);
}

SValue Crypto::random(int bytes) {
	Botan::AutoSeeded_RNG rng;
	Botan::byte rbytes[bytes];
	rng.randomize(rbytes, bytes);
	return SValue(rbytes, bytes);
}

/**
 * Retrieves a private key for a certain Host from  SAManagement
 */
PrivateKey* Crypto::getPrivateKey(IPAddress address) {
	//Get the private key
	const SecurityAssociation * privkeySA = _sam->getSA(SAprivkey, address);
	if (!privkeySA) {
		click_chatter("Could not find private key for host %s", address.unparse().c_str());
		return 0;
	}
	Botan::AutoSeeded_RNG aRng;
	Botan::DataSource_Memory src2((char *) privkeySA->myData);
	Botan::Private_Key * privkey =
			dynamic_cast<Botan::RSA_PrivateKey*>(Botan::PKCS8::load_key(src2,
					aRng, "testtesttest"));
	return privkey;
}

/**
 * Retrieves a public key for a certain Host from  SAManagement
 */
PublicKey* Crypto::getPublicKey(IPAddress address) {
	const SecurityAssociation * pubkeySA = _sam->getSA(SApubkey,address);
	if (!pubkeySA) {
		click_chatter("Could not find public key for host %s", address.unparse().c_str());
		return 0;
	}
	Botan::DataSource_Memory src((char *) pubkeySA->myData);
	Botan::Public_Key * pubkey =
		dynamic_cast<Botan::RSA_PublicKey*>(Botan::X509::load_key(src));
	return pubkey;
}

/**
 * Encrypt a value with a public key
 */
SValue Crypto::encrypt(SValue* plain, PublicKey* pubkey) {
	Botan::PK_Encryptor_EME encryptor(*pubkey, "EME1(SHA-256)");
	Botan::AutoSeeded_RNG aRng;
	Botan::SecureVector<Botan::byte> encrypted = encryptor.encrypt(*plain, aRng);

	//click_chatter("Encrypting: \t%s -> %s",
	//	CastorPacket::hexToString(plain->begin(), plain->size()).c_str(),
	//	CastorPacket::hexToString(encrypted.begin(), encrypted.size()).c_str());

	return encrypted;
}

/**
 * Decrypt a value with private key
 */
SValue Crypto::decrypt(SValue* cipher, PrivateKey* privkey) {
	Botan::PK_Decryptor_EME decryptor(*privkey, "EME1(SHA-256)");
	Botan::SecureVector<Botan::byte> decrypted = decryptor.decrypt(*cipher);

	//click_chatter("Decrypting: \t%s -> %s",
	//	CastorPacket::hexToString(cipher->begin(), cipher->size()).c_str(),
	//	CastorPacket::hexToString(decrypted.begin(), decrypted.size()).c_str());

	return decrypted;
}

/**
 * Return the symmetric shared key for a destination
 */
SymmetricKey* Crypto::getSharedKey(const IPAddress& address) {
	const SecurityAssociation* sharedKeySA = _sam->getSA(SAsharedsecret, address);
	if (!sharedKeySA) {
		return 0;
	}
	SymmetricKey* sharedKey = new Botan::OctetString(sharedKeySA->myData, sharedKeySA->mySize);
	return sharedKey;
}

/**
 * Encrypt plain using a block cipher. Add padding if length of plain is not multiple of block size.
 */
SValue Crypto::encrypt(const SValue& plain, const SymmetricKey& key) {
	blockCipher->set_key(key);

	size_t numBlocks = numberOfBlocks(blockCipher->block_size(), plain.size());
	size_t cipherLength = numBlocks * blockCipher->block_size();

	Botan::byte block[cipherLength];
	memcpy(block, plain.begin(), plain.size());
	blockCipher->encrypt_n(block, block, numBlocks);

	return SValue(block, cipherLength);
}

/**
 * Decrypt cipher using the given key. Note that you might have to remove padding that was added during encryption.
 */
SValue Crypto::decrypt(const SValue& cipher, const SymmetricKey& key) {
	blockCipher->set_key(key);

	size_t numBlocks = numberOfBlocks(blockCipher->block_size(), cipher.size());
	size_t cipherLength = numBlocks * blockCipher->block_size();

	Botan::byte block[cipherLength];
	memcpy(block, cipher.begin(), cipher.size());
	blockCipher->decrypt_n(block, block, numBlocks);

	return SValue(block, cipherLength);
}

SValue Crypto::hash(SValue data) {
	Botan::SHA_160 sha160;
	SValue hashed = sha160.process(data.begin(), data.size());
	return hashed;
}

void Crypto::testcrypt(SValue* plain, IPAddress dst) {
	PublicKey* pk = getPublicKey(dst);
	PrivateKey* sk = getPrivateKey(dst);

	if(!sk || !pk){
		click_chatter("Encryption error, no key for %s found", dst.unparse().c_str());
		return;
	}

	SValue encrypted = encrypt(plain, pk);
	SValue decrypted = decrypt(&encrypted, sk);
	click_chatter("Plaintext: \t%s", CastorPacket::hexToString(plain->begin(), plain->size()).c_str());
	click_chatter("Encrypt: \t%s", CastorPacket::hexToString(encrypted.begin(), encrypted.size()).c_str());
	click_chatter("Decrypt: \t%s", CastorPacket::hexToString(decrypted.begin(), decrypted.size()).c_str());

	if(plain->operator ==(decrypted)){
		click_chatter("Encryption successful");
	}
	else{
		click_chatter("Encryption failed");
	}
}

void Crypto::testSymmetricCrypt(SValue plain, IPAddress dst) {
	SymmetricKey* key = getSharedKey(dst);

	if (!key) {
		click_chatter("Encryption error, no key for %s found", dst.unparse().c_str());
		return;
	}

	SValue encrypted = encrypt(plain, *key);
	SValue decrypted = decrypt(encrypted, *key);
	click_chatter("Plaintext: \t%s", CastorPacket::hexToString(plain.begin(), plain.size()).c_str());
	click_chatter("Encrypt: \t%s",
			CastorPacket::hexToString(encrypted.begin(), encrypted.size()).c_str());
	click_chatter("Decrypt: \t%s",
			CastorPacket::hexToString(decrypted.begin(), decrypted.size()).c_str());

	if (plain.operator ==(SValue(decrypted.begin(), plain.size()))) {
		click_chatter("Encryption successful");
	} else {
		click_chatter("Encryption failed");
	}
}

CLICK_ENDDECLS
ELEMENT_LIBS(-L/usr/local/lib -lbotan-1.10)
EXPORT_ELEMENT(Crypto)
