#ifndef CLICK_CASTORHISTORY_HH
#define CLICK_CASTORHISTORY_HH
#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"
CLICK_DECLS

typedef uint8_t FlowId[CASTOR_HASHLENGTH];
typedef uint8_t PacketId[CASTOR_HASHLENGTH];
typedef uint8_t ACKAuth[CASTOR_HASHLENGTH];

typedef struct{
	FlowId 		flow;
	IPAddress		routedTo;
	Vector<IPAddress> ACKedBy;
}HistoryEntry;

typedef struct{
	ACKAuth 	aauth;
}ACKHistoryEntry;

class CastorTimeout;

class CastorHistory : public Element {
	public:
		CastorHistory();
		~CastorHistory();

		const char *class_name() const	{ return "CastorHistory"; }
		const char *port_count() const	{ return "0/0"; }
		const char *processing() const	{ return AGNOSTIC; }
		int configure(Vector<String>&, ErrorHandler*);

		void 	addToHistory(Packet*);
		bool 	checkDuplicate(Packet*);
		Packet*	getPacketById(PacketId);
		bool	hasACK(PacketId);
		bool 	ValidateACK(Packet*);
		bool 	IsFirstACK(Packet*);
		IPAddress PKTroutedto(Packet*);
		void GetFlowId(Packet*, FlowId*);

	private:
		void 	addACKToHistory(Packet*);
		void 	addPKTToHistory(Packet*);

		Vector<Packet*> 	_history;
		Vector<Packet*>  _ackhistory;
		Crypto* _crypto;
		CastorTimeout* _timeout;

		HashTable<String, HistoryEntry> _pkthistory;


};

CLICK_ENDDECLS
#endif
