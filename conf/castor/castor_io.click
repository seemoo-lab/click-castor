/**
 * Handle output to the Ethernet device
 */
elementclass OutputEth { 
	$broadcastJitter |

	prio :: PrioSched
		-> output

	highprio :: Queue -> [0] prio;
	lowprio :: Queue -> [1] prio;
	beacons_be :: Queue(CAPACITY 1) // best effort, size of one since we don't need beacon 'floods'
		-> recBeacon :: CastorRecordPkt
		-> [2] prio;

	input[0] // Castor PKT
		-> BroadcastJitter($broadcastJitter)
		-> lowprio;

	input[1] // Castor ACK
		-> BroadcastJitter($broadcastJitter)
		-> highprio;

	input[2] // Beacons
		-> beacons_be;
}

/**
 * Handle packets destined for local host
 *
 * Input(0): IP packet
 * Input(1): Other packet
 */
elementclass ToHost {

	input[0]
		-> CheckIPHeader
		-> output;

}

/**
 * Handle incoming packets from local host
 */
elementclass FromHost {
	$myIP |

	input
		-> CheckIPHeader
		-> SetIPSrc($myIP) // Packets coming from ns-3 tun0 (host) device have 127.0.0.1 set as source address
		-> output;

}
