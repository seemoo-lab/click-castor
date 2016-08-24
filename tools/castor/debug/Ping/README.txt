
===============================================================================
CASTOR PING
===============================================================================
By: symenschmitt@web.de
-------------------------------------------------------------------------------
Usage: ping <ip address> [options]
Options:
	-c | --count <x>: 	Stops after sending x packets.
	-i | --interval <x>: 	Wait x seconds between sending each packet. 
				The default is to wait for one second.
	-I | --Interface <x>: 	x is the name of the desired network interface. 
				The default interface is "wlan0".
	-p | --preloaded <x>: 	Sends x packets before trying to receive one. 
				The default is zero.
	-q | --quiet: 		Nothing is displayed except the summary lines.
	-s | --size <x>: 	Size of the transported data, in x byte. 
				The default is zero.
	-t | --timeout <x>: 	Time to wait for a response, in x second. 
				The default is one.
	-d | --deadline <x>: 	Specify a timeout, in x second, before ping exits.
	--ttl <x>: 		Amount of nodes that are allowed to forward 
				the packet. The default is the maximum value 
				of 15 nodes.

-------------------------------------------------------------------------------
Example 1:

> ./ping 192.168.56.102
PING 192.168.56.102
56 bytes from 192.168.56.102: time=1.81 ms
56 bytes from 192.168.56.102: time=1.73 ms
56 bytes from 192.168.56.102: time=3.01 ms
56 bytes from 192.168.56.102: time=2.82 ms
^C
--- 192.168.56.102 ping statistics ---
4 packets transmitted, 4 received, 0% packet loss, time 9.36ms
rtt min/avg/max = 1.73/2.34/3.01

-------------------------------------------------------------------------------
Example 2:

> ./ping 192.168.56.102 -I enp0s8 -c 4 -s 1337
PING 192.168.56.102
1337 bytes from 192.168.56.102: time=4.31 ms
1337 bytes from 192.168.56.102: time=5.48 ms
1337 bytes from 192.168.56.102: time=3.15 ms
From 192.168.56.101 Destination Host Unreachable

--- 192.168.56.102 ping statistics ---
4 packets transmitted, 3 received, 25% packet loss, time 12.94ms
rtt min/avg/max = 3.15/4.31/5.48

-------------------------------------------------------------------------------
