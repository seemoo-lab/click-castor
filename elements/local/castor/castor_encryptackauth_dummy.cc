#include <click/config.h>
#include <click/args.hh>
#include <click/confparse.hh>
#include "castor_encryptackauth_dummy.hh"
#include "crypto.hh"
#include <botan/data_src.h>
#include <click/straccum.hh>




CLICK_DECLS
CastorEncryptACKAuthDummy::CastorEncryptACKAuthDummy(){}

CastorEncryptACKAuthDummy::~ CastorEncryptACKAuthDummy(){}

int CastorEncryptACKAuthDummy::configure(Vector<String> &conf, ErrorHandler *errh)
{
	int res = cp_va_kparse(conf, this, errh,
//		"SAM", cpkP+cpkM, cpElementCast, "SAManagement", &_sam,
		"CRYPT", cpkP+cpkM, cpElementCast, "Crypto", &_crypto,
		"ADDR", cpkP+cpkM, cpIPAddress, &_myIP,
		cpEnd);
	if(res < 0) return res;
	return 0;
}


void CastorEncryptACKAuthDummy::push(int, Packet *p){

	output(0).push(p);

}

//int CastorEncryptACKAuthDummy::encrypt(uint8_t * toEnc,int toEncLength,uint8_t * enc, IPAddress destination){
//int CastorEncryptACKAuthDummy::encrypt(uint8_t * toEnc,int toEncLength,uint8_t * enc, IPAddress destination){
//
//
//	//SecurityAssociation * privkeySA =_sam->getSA(SAprivkey,_myIP);
//	SecurityAssociation * pubkeySA = _sam->getSA(SApubkey,destination);
//
//	if(!pubkeySA){
//		click_chatter("dont have public key for the destination ");
//		return -1;
//	}
//
//	Botan::AutoSeeded_RNG aRng;
//	Botan::DataSource_Memory src2((char *)pubkeySA->myData);
//	//Botan::Public_Key * pubkey=dynamic_cast<Botan::RSA_PublicKey*>(Botan::PKCS8::load_key(src2,aRng,"testtesttest"));
//	Botan::Public_Key * pubkey = dynamic_cast<Botan::RSA_PublicKey*>(Botan::X509::load_key(src2));
//
//	//"EMSA4(SHA-160)" (RSA-PSS) is the signature scheme: hash with SHA-160 then sign the hash
//	//Botan::PK_Signer signer(*privkey,"EMSA4(SHA-160)");
//
//
//	Botan::PK_Encryptor_EME encryptor(*pubkey, "EME1(SHA-1)");
//
//	Botan::SecureVector<Botan::byte> encrypted;
//
//	encrypted = encryptor.encrypt((Botan::byte*) toEnc, toEncLength, aRng);
//
//	//memcpy(enc, encrypted.begin(), encrypted.size());
//
//	//Debug
//	StringAccum sa;
//	sa << "Encrypting ACK Authenticator, size " << encrypted.size() << " ";
//	sa << CastorPacket::hexToString(toEnc,toEncLength);
//	sa << " -> ";
//	sa << CastorPacket::hexToString(encrypted.begin(),encrypted.size());
//	click_chatter(sa.c_str());
//
//	click_chatter("FOO");
//	return 0;
//
//
//}

////void CastorEncryptACKAuthDummy::testcrypt(uint8_t * toEnc,int toEncLength, IPAddress destination){
//void CastorEncryptACKAuthDummy::testcrypt(Botan::SecureVector<Botan::byte> plain, IPAddress destination){
//
//	//Botan::LibraryInitializer();
//
//	Botan::Public_Key * pubkey = getPublicKey(IPAddress("1.0.0.2"));
//	click_chatter("Have a public key with %s", pubkey->algo_name().c_str());
//
//	Botan::Private_Key * privkey = getPrivateKey(IPAddress("1.0.0.4"));
//	click_chatter("Have a private key with %s", privkey->algo_name().c_str());
//
//	//Botan::PK_Encryptor* rsa_enc = Botan::get_pk_encryptor(*pubkey, "EME1(SHA-256)");
//
//	Botan::PK_Encryptor_EME encryptor(*pubkey, "EME1(SHA-256)");
//	Botan::AutoSeeded_RNG aRng;
//	Botan::SecureVector<Botan::byte> encrypted = encryptor.encrypt(plain, aRng);
//	click_chatter("Encrypting %s", CastorPacket::hexToString(plain.begin(),plain.size()).c_str() );
//	click_chatter("Encrypted %s", CastorPacket::hexToString(encrypted.begin(),encrypted.size()).c_str() );
//
//	Botan::PK_Decryptor_EME decryptor(*privkey, "EME1(SHA-256)");
//	Botan::SecureVector<Botan::byte> decrypted = decryptor.decrypt(encrypted);
//	click_chatter("Decrypted %s", CastorPacket::hexToString(decrypted.begin(),decrypted.size()).c_str() );
//
//	click_chatter("Finished");
//
//}
//bool Crypto::verify(IPAddress signer, uint8_t * toVerify, int toVerifyLength, uint8_t * sig, int sigLength){
//
//	//Botan::Public_Key * pubkey=myKeyStore->getPublicKey(signer);
//
//	//mySAM->printSAs();
//	SecurityAssociation * pubkeySA=mySAM->getSA(SApubkey,signer);
//
//	if(!pubkeySA){//we don't know a public key for this IP
//		click_chatter("no key here...");
//		return false;
//	}
//	else
//	{
//		Botan::DataSource_Memory src((char *)pubkeySA->myData);
//		Botan::Public_Key * pubkey = dynamic_cast<Botan::RSA_PublicKey*>(Botan::X509::load_key(src));
//
//		Botan::PK_Verifier aVerifier(*pubkey,"EMSA4(SHA-160)");
//
//		return aVerifier.verify_message( (Botan::byte *)toVerify ,toVerifyLength,sig ,sigLength);
//	}
//
//
//}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorEncryptACKAuthDummy)

