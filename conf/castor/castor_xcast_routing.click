require(
	library castor_settings.click,
	library castor_io.click,
	library castor_common.click,
	library castor_xcast.click,
	library castor_init_common_blocks.click
);

// How to choose next hop
routeselector :: CastorRouteSelectorOriginal(routingtable, neighbors, $broadcastAdjust);

// How to handle PKTs and ACKs
handlepkt :: CastorHandleXcastPkt(fake, routeselector, routingtable, timeouttable, ratelimits, history, crypto);
handleack :: CastorHandleAck(fake, routingtable, timeouttable, ratelimits, history, neighbors, crypto);

handleIpPacket :: CastorHandleMulticastIpPacket(fake, flowmanager, crypto, groupmap);
removeHeader :: CastorXcastRemoveHeader;

// Finally wire all blocks
require(
	library castor_wiring.click,
);
