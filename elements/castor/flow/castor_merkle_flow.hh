#ifndef CLICK_CASTOR_MERKLE_FLOW_HH
#define CLICK_CASTOR_MERKLE_FLOW_HH

#include <click/vector.hh>
#include "../crypto/crypto.hh"
#include "castor_flow_table.hh"
#include "castor_flow_entry.hh"

CLICK_DECLS

class CastorFlowManager;

class PacketLabel {
public:
	PacketLabel(unsigned int num, unsigned int size, const FlowId &fid, const PacketId &pid, const Nonce &n)
			: num(num), size(size), fid(fid), pid(pid), n(n) { }
	unsigned int num;
	unsigned int size;
	const FlowId &fid;
	const PacketId &pid;
	const Nonce &n;
};

class CastorMerkleFlow {
public:
	CastorMerkleFlow(size_t size, const NodeId& dst, CastorFlowTable* flowtable, const Crypto* crypto);
	PacketLabel freshLabel();
	const FlowId &getFlowId() const;
	bool isAlive() const;
private:
	friend class CastorFlowManager;
	// Dummy constructor
	CastorMerkleFlow() : entry(NULL), pos(0) { }

	CastorFlowEntry *entry;
	unsigned int pos;
};

CLICK_ENDDECLS

#endif
