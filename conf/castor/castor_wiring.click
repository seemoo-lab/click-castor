/*******************
 * Wire the Blocks *
 *******************/

ethin[1] -> ethout;			// Push new ARP Responses back to device
ethin[0] -> [1]arpquerier;	// Push incoming ARP responses to querier
ethin[2]
	-> cEtherFilter :: CastorEtherFilter
	-> castorclassifier;	// Classify received packets

cEtherFilter[1] // Received beacon from neighbor
	-> AddNeighbor(neighbors, $neighborsEnable)
	-> Discard;

arpquerier -> ethout;	// Send Ethernet packets to output

fromhost
	-> handleIpPacket
	-> handlepkt;		// Process new generated packets

castorclassifier[0] -> handlepkt; // Process PKTs
castorclassifier[1] -> handleack; // Process ACKs
castorclassifier[2] -> [1]tohost; // Deliver non-Castor packets directly to host

handlepkt[0]		-> removeHeader -> [0]tohost; // Deliver PKT to host
handlepkt[1]		-> arpquerier; // Return ACK
handlepkt[2]		-> arpquerier; // Forward PKT
handleack			-> arpquerier; // Forward ACK

NeighborBeaconGenerator($beaconingInterval, fake, $EthDev, $neighborsEnable) -> ethout;
