#include <click/config.h>
#include <click/confparse.hh>
#include "castor_addtohistory.hh"

CLICK_DECLS

CastorAddToHistory::CastorAddToHistory() {
}

CastorAddToHistory::~CastorAddToHistory() {
}

int CastorAddToHistory::configure(Vector<String> &conf, ErrorHandler *errh) {
     return cp_va_kparse(conf, this, errh,
        "CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &_history,
        "StartTimerUponAdding", cpkP+cpkM, cpBool, &_startTimer,
        cpEnd);
}

void CastorAddToHistory::push(int, Packet *p){
	_history->addToHistory(p, _startTimer);

	output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorAddToHistory)
