/**
 * Handle output to the Ethernet device
 */
elementclass OutputEth{ 
	$myEthDev, $jitter |

	input[0]
		-> Queue
		-> JitterUnqueue($jitter, true) // 'true' set for simulator -> much better performance
		-> ethdev :: ToSimDevice($myEthDev);
}

/**
 * Handle incoming packets on Ethernet device
 */
elementclass InputEth{
	$myEthDev, $myAddr |

	ethdev :: FromSimDevice($myEthDev, SNAPLEN 4096)
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
 * Handle packets destined for local host
 *
 * Input(0): IP packet
 * Input(1): Other packet
 */
elementclass ToHost {
	$myHostDev |
	
	hostdevice :: ToSimDevice($myHostDev, IP);

	input[0]
		-> CheckIPHeader2
		-> MarkIPHeader
		-> hostdevice;

	input[1]
		-> hostdevice;

}

/**
 * Handle incoming packets from local host
 */
elementclass FromHost {
	$myHostDev, $myIP |

	fromhost :: FromSimDevice($myHostDev, SNAPLEN 4096)
		-> CheckIPHeader2(VERBOSE true)
		-> MarkIPHeader
		-> CastorTranslateLocalhost($myIP) // Packets coming from host have 127.0.0.1 set as source address, so replace with address of local host
		-> output;
		
}