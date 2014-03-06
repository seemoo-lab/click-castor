#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "crypto.hh"

#include <botan/hash.h>
#include <botan/sha160.h>
#include <botan/auto_rng.h>
#include <botan/rsa.h>
#include <botan/pubkey.h>

CLICK_DECLS
Crypto::Crypto(){}

Crypto::~ Crypto(){}

/*
 * Configure the Crypto Element
 */
int Crypto::configure(Vector<String> &conf, ErrorHandler *errh)
{
	int res = cp_va_kparse(conf, this, errh,
		"SAM", cpkP+cpkM, cpElementCast, "SAManagement", &_sam,
		cpEnd);
	if(res < 0) return res;
	return 0;
}

void Crypto::hash(Hash* hash, uint8_t* data, uint8_t length) {

	Botan::SHA_160 sha160;
	Botan::SecureVector<Botan::byte> hashed = sha160.process(data, length);
	memcpy(hash, hashed.begin(), CASTOR_HASHLENGTH);
}

void Crypto::randomize(Hash* r) {
	Botan::AutoSeeded_RNG rng;
	rng.randomize((uint8_t*) r, CASTOR_HASHLENGTH);
}

//static SHash Crypto::random(int bytes) {
//	Botan::AutoSeeded_RNG rng;
//	Botan::byte rbytes[bytes];
//	rng.randomize(rbytes, bytes);
//	return SHash(rbytes, bytes);
//}
SValue Crypto::random(int bytes) {
	Botan::AutoSeeded_RNG rng;
	Botan::byte rbytes[bytes];
	rng.randomize(rbytes, bytes);
	return SValue(rbytes, bytes);
}

/**
 * Retrieves a private key for a certain Host from  SAManagement
 */
Private_Key* Crypto::getPrivateKey(IPAddress address) {
	//Get the private key
	SecurityAssociation * privkeySA = _sam->getSA(SAprivkey, address);
	if (!privkeySA) {
		click_chatter("Error, dont have private key for the destination ");
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
Public_Key* Crypto::getPublicKey(IPAddress address) {
	SecurityAssociation * pubkeySA = _sam->getSA(SApubkey,address);
	if (!pubkeySA) {
		click_chatter("Error, dont have public key for the destination ");
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
SValue Crypto::encrypt(SValue* plain, Public_Key* pubkey) {
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
SValue Crypto::decrypt(SValue* cipher, Private_Key* privkey) {
	Botan::PK_Decryptor_EME decryptor(*privkey, "EME1(SHA-256)");
	Botan::SecureVector<Botan::byte> decrypted = decryptor.decrypt(*cipher);

	//click_chatter("Decrypting: \t%s -> %s",
	//	CastorPacket::hexToString(cipher->begin(), cipher->size()).c_str(),
	//	CastorPacket::hexToString(decrypted.begin(), decrypted.size()).c_str());

	return decrypted;
}

//static SHash Crypto::hash(SHash data) {
//	Botan::SHA_160 sha160;
//	SHash hashed = sha160.process(data.begin(), data.size());
//	return hashed;
//}
SValue Crypto::hash(SValue data) {
	Botan::SHA_160 sha160;
	SValue hashed = sha160.process(data.begin(), data.size());
	return hashed;
}

void Crypto::testcrypt(SValue* plain, IPAddress dst) {
	Public_Key* pk = getPublicKey(dst);
	Private_Key* sk = getPrivateKey(dst);

	if(!sk || !pk){
		click_chatter("Encryption error, no keys found");
		return;
	}

	SValue encrypted = encrypt(plain, pk);
	SValue decrypted = decrypt(&encrypted, sk);
	click_chatter("Plain: \t%s", CastorPacket::hexToString(plain->begin(), plain->size()).c_str());
	click_chatter("Encrypt: \t%s", CastorPacket::hexToString(encrypted.begin(), encrypted.size()).c_str());
	click_chatter("Decrypt: \t%s", CastorPacket::hexToString(decrypted.begin(), decrypted.size()).c_str());

	if(plain->operator ==(decrypted)){
		click_chatter("Encryption successfull");
	}
	else{
		click_chatter("Encryption failed");
	}
}

CLICK_ENDDECLS
ELEMENT_LIBS(-L/usr/local/lib -lbotan-1.10)
EXPORT_ELEMENT(Crypto)
