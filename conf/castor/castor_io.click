/**
 * Delays MAC layer broadcast frames by a random jitter
 */
elementclass BroadcastJitter {
	$broadcastJitter, $isSimulator |
	
	input
		-> cp :: CheckPaint(10, ANNO 32) // this frame was broadcasted -> jitter transmission (Paint(10))
		-> Queue
		-> JitterUnqueue($broadcastJitter, $isSimulator) // 'true' set for simulator -> much better performance
		-> output;
	
	cp[1]
		-> output; // this frame was not broadcasted -> no need to delay (Paint(0))
}

/**
 * Handle output to the Ethernet device
 */
elementclass OutputEth { 
	$myEthDev, $broadcastJitter |

	prio :: PrioSched
		-> ethdev :: ToSimDevice($myEthDev);

	highestprio :: Queue -> [0] prio;
	highprio :: Queue -> [1] prio;
	lowprio :: Queue -> [2] prio;
	beaconQueue :: Queue(CAPACITY 1) -> [3] prio; // lowest priority, size of one since we don't need beacon 'floods'

	arpquerier :: ARPQuerier(fake, TIMEOUT 3600, POLL_TIMEOUT 0); // Set timeout sufficiently long, so we don't introduce ARP overhead (we set entries in ns-3)
	arpquerier[1] -> highestprio; // TODO: not sure if ARP requests and replies should be highest prio

	arpquerier[0] -> ps :: PaintSwitch -> lowprio;
	// -> StoreEtherAddress(ff:ff:ff:ff:ff:ff, dst) // Force send packet as MAC layer broadcast (no retransmissions, no RTS/CTS, lowest transmission rate)
	ps[1] -> highprio;

	input[0] // Castor PKT
		-> BroadcastJitter($broadcastJitter, true)
		-> Paint(0)
		-> arpquerier;

	input[1] // Castor ACK
		-> BroadcastJitter($broadcastJitter, true)
		-> Paint(1)
		-> arpquerier;

	input[2] // Beacons
		-> beaconQueue;

	input[3] // ARP reply (answer to one of our requests)
		-> [1]arpquerier;

	input[4] // ARP reply from us (directly send out)
		-> highestprio;

}

/**
 * Handle incoming packets on Ethernet device
 */
elementclass InputEth {
	$myEthDev, $myAddr |

	ethdev :: FromSimDevice($myEthDev, SNAPLEN 4096, PROMISC true) // promiscuous mode, so beacons can be received 
																   // no PROMISC mode seems to prevent non IP and ARP ethertypes to come through
		-> HostEtherFilter($myEthDev)
		-> arpclassifier :: Classifier(12/0806 20/0001, 12/0806 20/0002, -); // Filter ARP messages (Request / Reply / Default)

	arpclassifier[0] // Handle ARP request
		-> ARPResponder($myAddr)
		-> [1]output;

	arpclassifier[1] // Handle ARP response
		-> [0]output;

	arpclassifier[2] // Handle default
		-> [2]output;
}

/**
 * Handle incoming packets on Ethernet device, packets in output 2 are not filtered for dest address
 */
elementclass InputEthNoHostFilter {
	$myEthDev, $myAddr |

	ethdev :: FromSimDevice($myEthDev, SNAPLEN 4096, PROMISC true)
		-> arpclassifier :: Classifier(12/0806 20/0001, 12/0806 20/0002, -); // Filter ARP messages (Request / Reply / Default)

	arpclassifier[0] // Handle ARP request
		-> HostEtherFilter($myEthDev)
		-> ARPResponder($myAddr)
		-> [1]output;

	arpclassifier[1] // Handle ARP response
		-> HostEtherFilter($myEthDev)
		-> [0]output;

	arpclassifier[2] // Handle default
		-> [2]output;

}

/**
 * Handle packets destined for local host
 *
 * Input(0): IP packet
 * Input(1): Other packet
 */
elementclass ToHost {
	$myHostDev |
	
	hostdevice :: ToSimDevice($myHostDev, IP);

	input[0]
		-> CheckIPHeader
		-> hostdevice;

	input[1]
		-> hostdevice;

}

/**
 * Handle incoming packets from local host
 */
elementclass FromHost {
	$myHostDev, $myIP, $headroom |

	fromhost :: FromSimDevice($myHostDev, SNAPLEN 4096, HEADROOM $headroom)
		-> CheckIPHeader
		-> SetIPSrc($myIP) // Packets coming from ns-3 tun0 (host) device have 127.0.0.1 set as source address
		-> output;
		
}
