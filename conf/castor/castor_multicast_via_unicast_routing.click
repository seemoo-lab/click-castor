require(
	library castor_settings.click,
	library castor_io.click,
	library castor_common.click,
	library castor_unicast.click,
	library castor_init_common_blocks.click,
);

// How to choose next hop
routeselector :: CastorRouteSelectorOriginal(routingtable, neighbors, $broadcastAdjust);

// How to handle PKTs and ACKs
handlepkt :: CastorHandlePkt(fake, routeselector, routingtable, flowtable, timeouttable, ratelimits, history, crypto);
handleack :: CastorHandleAck(fake, routingtable, flowtable, timeouttable, ratelimits, history, neighbors, crypto);

handleIpPacket :: CastorHandleMulticastToUnicastIpPacket(fake, flowmanager, flowtable, crypto, groupmap);
removeHeader :: CastorRemoveHeader;

// Finally wire all blocks
require(
	library castor_wiring.click,
);
