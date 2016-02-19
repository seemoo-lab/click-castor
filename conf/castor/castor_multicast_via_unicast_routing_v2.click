define (
	$CASTOR_VERSION 2,
);

require(
	library castor_settings.click,
	library castor_io.click,
	library castor_common.click,
	library castor_unicast.click,
	library castor_init_common_blocks.click,
);

// How to choose next hop
routeselector :: CastorRouteSelectorExperimental(routingtable, neighbors, history, $broadcastAdjust);

// How to handle PKTs and ACKs
handlepkt :: CastorHandlePkt(fake, routeselector, routingtable, timeouttable, nextflowtable, ratelimits, history, crypto);
handleack :: { input -> handleCastorAck :: CastorHandleAck(fake, routingtable, timeouttable, nextflowtable, ratelimits, history, neighbors, crypto, true) -> CastorXcastResetDstAnno(true) -> output; };

handleIpPacket :: CastorHandleMulticastToUnicastIpPacket(fake, flowmanager, crypto);
removeHeader :: CastorRemoveHeader;

// Finally wire all blocks
require(
	library castor_wiring.click,
);
