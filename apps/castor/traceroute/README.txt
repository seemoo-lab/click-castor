===============================================================================
CASTOR TRACEROUTE 
===============================================================================
By: symenschmitt@web.de
-------------------------------------------------------------------------------
Usage: traceroute <ip address> [options]
Options:
	-e | --extensive <x>: 	Consider all involved nodes.
	-I | --Interface <x>: 	x  is the name of the desired network interface. 
				The default interface is "wlan0".
	-t | --timeout <x>: 	Time to wait for a response, in x second. 
				The default is one.
	-d | --deadline <x>: 	Specify a timeout, in x second, before ping exits.
	--ttl <x>: 		Amount of nodes that are allowed to forward 
				the packet. The default is the maximum value 
				of 15 nodes.
	--route [all|dst|nodst]: Show all nodes or with(out) destination. 
				The default is dst.
	--address\_type [ip|mac|ip\_mac]: Show node entry as IP, MAC or both. 
				The default is ip.
	--sort [normal|up|down]: Shows the routes in the desired order. The default 
				is normal and does no sorting.

-------------------------------------------------------------------------------
Example 1:

> ./traceroute 192.168.56.102
TRACEROUTE
==============================================

-- 192.168.56.101 ==> 192.168.56.102 --

==============================================
time: 1.63ms | size: 102byte | nodes: 3
----------------------------------------------
    192.168.56.100 | 0.00ms
--> 192.168.56.101 | 0.93ms
--> 192.168.56.102 | 1.63ms

==============================================

-------------------------------------------------------------------------------
Example 2:

> ./traceroute 192.168.56.102 -I enp0s8 -e -d 5 \ 
--route all --address_type ip_mac --sort up
TRACEROUTE
==============================================

-- 192.168.56.101 ==> 192.168.56.102 --
^C
==============================================
time: 0.53ms | size: 102byte | nodes: 2
----------------------------------------------
    192.168.56.100(08-00-27-CC-77-50) | 0.00ms
--> 192.168.56.104(08-00-27-CC-77-54) | 0.53ms

==============================================
time: 1.63ms | size: 102byte | nodes: 3
----------------------------------------------
    192.168.56.100(08-00-27-CC-77-50) | 0.00ms
--> 192.168.56.101(08-00-27-CC-77-51) | 0.93ms
--> 192.168.56.102(08-00-27-CC-77-52) | 1.63ms

==============================================
time: 2.20ms | size: 102byte | nodes: 4
----------------------------------------------
    192.168.56.100(08-00-27-CC-77-50) | 0.00ms
--> 192.168.56.103(08-00-27-CC-77-53) | 0.20ms
--> 192.168.56.101(08-00-27-CC-77-51) | 1.00ms
--> 192.168.56.102(08-00-27-CC-77-52) | 2.20ms

==============================================

-------------------------------------------------------------------------------

