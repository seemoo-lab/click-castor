#ifndef CLICK_CASTOR_HISTORY_HH
#define CLICK_CASTOR_HISTORY_HH

#include <click/element.hh>
#include "castor.hh"
#include "crypto.hh"

CLICK_DECLS

typedef struct{
	FlowId 		flow;
	IPAddress		routedTo;
	Vector<IPAddress> ACKedBy;
}HistoryEntry;

typedef struct{
	ACKAuth 	aauth;
}ACKHistoryEntry;

typedef long Key; // FIXME currently using only part of pid as key

class CastorHistory : public Element {
	public:
		CastorHistory();
		~CastorHistory();

		const char *class_name() const	{ return "CastorHistory"; }
		const char *port_count() const	{ return PORTS_0_0; }
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
		inline Key getKeyForPacket(ACKAuth);
		inline HistoryEntry* getEntryForAuth(ACKAuth);
		inline HistoryEntry* getEntryForPid(PacketId);

		Vector<Packet*> 	_history;
		Vector<Packet*>  _ackhistory;
		Crypto* _crypto;
		HashTable<Key, HistoryEntry> _pkthistory;

};

CLICK_ENDDECLS

#endif
