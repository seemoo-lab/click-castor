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
	PacketId 	packet;
	IPAddress	routedTo;
}HistoryEntry;

typedef struct{
	ACKAuth 	aauth;
}ACKHistoryEntry;

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

	private:
		Vector<Packet*> 	_history;
		Vector<Packet*>  _ackhistory;
		Crypto* _crypto;

};

CLICK_ENDDECLS
#endif
