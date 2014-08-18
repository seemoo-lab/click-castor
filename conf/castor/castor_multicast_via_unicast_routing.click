require(
	library castor_io.click,
	library castor_settings.click,
	library castor_common.click,
	library castor_unicast.click,
);

/*************************
 * Initialize the Blocks *
 *************************/

ethin :: InputEth($EthDev, fake);
ethout :: OutputEth($EthDev, $broadcastJitter, $unicastJitter);
fromhost :: FromHost($HostDev, fake);
tohost :: ToHost($HostDev);

sam::SAManagement(fake, netAddr, 100);
crypto::Crypto(sam);
flowDB :: CastorFlowStub;
flow_merkle :: CastorFlowMerkle(flowDB, crypto);
routingtable :: CastorRoutingTable($broadcastAdjust, $updateDelta);
history :: CastorHistory;
castorclassifier :: CastorClassifier;
handlepkt :: CastorHandlePkt(fake, routingtable, history, crypto);
handleack :: CastorHandleAck(fake, routingtable, history, crypto);

handleIpPacket :: CastorHandleMulticastToUnicastIpPacket(fake, flowDB, crypto);
arpquerier :: ARPQuerier(fake, TIMEOUT 3600); // Set timeout sufficiently long, so we don't introduce ARP overhead (we set entries in ns-3)


/*******************
 * Wire the Blocks *
 *******************/

ethin[1] -> ethout;		// PUSH new ARP Responses back to device
ethin[0] -> [1]arpquerier;	// Push incoming arp responses to querer
ethin[2]
	-> removeEthernetHeader :: Strip(14)
 	-> castorclassifier;	// Classify received packets			
 
arpquerier -> ethout; // Send Ethernet packets to output

fromhost
	-> handleIpPacket 
	-> handlepkt;		// Process new generated packets
 

castorclassifier[0] -> handlepkt; // Process PKTs
castorclassifier[1] -> handleack; // Process ACKs
castorclassifier[2] -> [1]tohost; // Deliver non-Castor packets directly to host

handlepkt[0]		-> CastorRemoveHeader -> [0]tohost;  // Deliver PKT to host
handlepkt[1]		-> arpquerier; // Return ACK		
handlepkt[2]		-> arpquerier; // Forward PKT
handleack		-> arpquerier; // Forward ACK
