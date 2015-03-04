/**
 * Delays MAC layer broadcast frames by '$jitter' milliseconds
 */
elementclass BroadcastDelayer {
	$broadcastJitter, $unicastJitter, $isSimulator |
	
	input[0]
		-> dstFilter :: Classifier(0/ffffffffffff, -);
		
	dstFilter[0]
		-> Queue
		-> JitterUnqueue($broadcastJitter, $isSimulator) // 'true' set for simulator -> much better performance
		-> output;
	
	dstFilter[1]
		-> Queue
		-> JitterUnqueue($unicastJitter, $isSimulator) // 'true' set for simulator -> much better performance
		-> output;
	
}

/**
 * Handle output to the Ethernet device
 */
elementclass OutputEth { 
	$myEthDev, $broadcastJitter, $unicastJitter |

	input[0]
		-> BroadcastDelayer($broadcastJitter, $unicastJitter, true)
		-> Queue
		-> ethdev :: ToSimDevice($myEthDev);

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
		-> CastorTranslateLocalhost($myIP) // Packets coming from ns-3 tun0 (host) device have 127.0.0.1 set as source address
		-> SetIPChecksum
		-> output;
		
}