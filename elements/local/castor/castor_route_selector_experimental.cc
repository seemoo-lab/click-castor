#include <click/config.h>
#include <click/confparse.hh>
#include "castor_route_selector_experimental.hh"

CLICK_DECLS

int CastorRouteSelectorExperimental::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &routingtable,
			"CastorNeighbors", cpkP+cpkM, cpElementCast, "CastorNeighbors", &neighbors,
			"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
			"BroadcastAdjust", cpkP, cpDouble, &broadcastAdjust,
			cpEnd);
}

void CastorRouteSelectorExperimental::selectNeighbor(const IPAddress &entry, double entryEstimate, Vector<IPAddress> &bestEntries, double &bestEstimate, const PacketId &pid)
{
	(void)pid;  // we do not make use of the packet identifier in selecting neighbors, but other selector classes may

	if (!history->hasPktFrom(pid, entry)) {
		if (entryEstimate > bestEstimate) {
			bestEntries.clear();
			bestEntries.push_back(entry);
			bestEstimate = entryEstimate;
		} else if (entryEstimate >= bestEstimate) {
			bestEntries.push_back(entry);
		}
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRouteSelectorExperimental)
