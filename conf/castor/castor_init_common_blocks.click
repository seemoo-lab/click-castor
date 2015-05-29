/****************************
 * Initialize common blocks *
 ****************************/

sam::SAManagement(fake);
crypto::Crypto(sam);

neighbors :: Neighbors($neighborTimeout, $neighborsEnable);
history :: CastorHistory;
routingtable :: CastorRoutingTable($updateDelta);
timeouttable :: CastorTimeoutTable(INIT $initTo, MIN $minTo, MAX $maxTo);

castorclassifier :: CastorClassifier(fake, neighbors);

ethin :: InputEthNoHostFilter($EthDev, fake);
ethout :: OutputEth($EthDev, $broadcastJitter);
fromhost :: FromHost($HostDev, fake, $headroom);
tohost :: ToHost($HostDev);
