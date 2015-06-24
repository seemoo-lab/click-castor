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

fromextdev -> ethin :: InputEthNoHostFilter($EthDev, fake);
ethout :: OutputEth($broadcastJitter) -> toextdev;
fromhostdev -> fromhost :: FromHost(fake);
tohost :: ToHost() -> tohostdev;
