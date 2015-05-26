require(
	library castor_settings.click,
	library castor_io.click,
	library castor_common.click,
	library castor_xcast.click,
	library castor_init_common_blocks.click
);

// The flow to use
flowDB :: CastorFlowStub;
flow_merkle :: CastorFlowMerkle(flowDB, crypto);

// How to choose next hop
routeselector :: CastorRouteSelectorOriginal(routingtable, neighbors, $broadcastAdjust);

// How to handle PKTs and ACKs
handlepkt :: {
	input
		-> handleXcastPkt :: CastorHandleXcastPkt(fake, routeselector, routingtable, history, crypto)[0,1]
		=> [0,1]output;
	handleXcastPkt[2] -> CastorXcastResetDstAnno(true) -> [2]output;
};
handleack :: { input -> handleXcastAck :: CastorHandleAck(fake, routingtable, history, neighbors, crypto, true) -> CastorXcastResetDstAnno(true) -> output; };

handleIpPacket :: CastorHandleMulticastIpPacket(fake, flowDB, crypto);
removeHeader :: CastorXcastRemoveHeader;

// Finally wire all blocks
require(
	library castor_wiring.click,
);
