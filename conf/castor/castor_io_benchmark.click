// I/O interfaces for local benchmark
AddressInfo(fake 1.2.3.4 C0-FF-EE-C0-FF-EE);

fromhostdev :: { RatedSource(LENGTH 1024, RATE 100000000, LIMIT -1) -> IPEncap(4, 1.2.3.4, 4.3.2.1) -> output };
tohostdev :: Discard;
fromextdev :: Idle;
toextdev :: Unqueue
	-> Classifier(12/88B6, -)
	=> ( input[0] -> Strip(22) -> CastorRecordPkt(VERBOSE true) -> output; 
	     input[1] -> output; )
	-> Discard;

elementclass BroadcastJitter {
	$broadcastJitter |

	input -> output;
}
