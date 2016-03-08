// I/O interfaces for local benchmark
define(
	$EthDev eth0,
);

AddressInfo(fake $EthDev);

fromhostdev :: { RatedSource(LENGTH 1024, RATE 100000000, LIMIT -1) -> IPEncap(4, 1.2.3.4, 4.3.2.1) -> output };
tohostdev :: Discard;
fromextdev :: Idle;
toextdev :: Discard;

elementclass BroadcastJitter {
	$broadcastJitter |

	input -> output;
}
