#include <click/config.h>
#include <click/confparse.hh>
#include "castor_route_selector_experimental.hh"

CLICK_DECLS

int CastorRouteSelectorExperimental::configure(Vector<String> &conf, ErrorHandler *errh) {
	return cp_va_kparse(conf, this, errh,
			"CastorRoutingTable", cpkP+cpkM, cpElementCast, "CastorRoutingTable", &routingtable,
			"Neighbors", cpkP+cpkM, cpElementCast, "Neighbors", &neighbors,
			"CastorHistory", cpkP+cpkM, cpElementCast, "CastorHistory", &history,
			"BroadcastAdjust", cpkP, cpDouble, &broadcastAdjust,
			cpEnd);
}

bool CastorRouteSelectorExperimental::selectNeighbor(const NodeId &entry, double entryEstimate, Vector<NodeId> &bestEntries, double &bestEstimate, const PacketId &pid)
{
#if 1
	if (!history->hasPkt(pid) || entry != history->getPktSenders(pid)[0])
#elif 0
	if (!history->hasPktFrom(pid, entry))
#else
	(void)pid;
#endif
	{
		if (entryEstimate > bestEstimate) {
			bestEntries.clear();
			bestEntries.push_back(entry);
			bestEstimate = entryEstimate;
		} else if (entryEstimate >= bestEstimate && entryEstimate > 0) {
			bestEntries.push_back(entry);
		}
		return true;
	}
	return false;
}

NodeId CastorRouteSelectorExperimental::chooseNeighbor(Vector<NodeId> &bestNeighbors, double best, const PacketId &pid)
{
#if 0
	if (history->hasPkt(pid)) {
		const IPAddress firstPktSender = history->getPktSenders(pid)[0];
		for (int i = 0; i < bestNeighbors.size(); i++) {
			if (bestNeighbors[i] == firstPktSender)
				return selectDefault();
		}
	}
#endif
	return CastorRouteSelectorOriginal::chooseNeighbor(bestNeighbors, best, pid);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(CastorRouteSelectorExperimental)
