============================
CASTOR DEBUG TOOLS - Alpha -
============================
symenschmitt@web.de
----------------------------

This Version contains changes in the following files:
<path to click-castor>/
	./conf/castor/
		castor_common.click
		castor_init_common_blocks.click
		castor_unicast.click
		castor_wiring.click
	./elements/castor/
		castor.hh
	./elements/castor/debug/
		castor_create_debug_ack.cc
	        castor_debug_authenticate_ack.hh 
		castor_debug_print.cc  
		castor_is_aret.hh
		castor_create_debug_ack.hh   
		castor_debug_handler.cc
           	castor_debug_print.hh
	  	castor_is_dbg.cc
		castor_create_debug_pkt.cc        
		castor_debug_handler.hh           
		castor_insert_path.cc  
		castor_is_dbg.hh
		castor_create_debug_pkt.hh        
		castor_debug_no_loopback.cc       
		castor_insert_path.hh  
		castor_is_insp.cc
		castor_debug_authenticate_ack.cc  
		castor_debug_no_loopback.hh       
		castor_is_aret.cc      
		castor_is_insp.hh	
	./tools/castor/debug/
		cli.cc
		cli.hh
		ping.cc
		ping.hh

---------------------------------------------------------------------------------------------------

Abstract:
=========

The extensions of the Castor protocol should already...

	... read/write strings from a UNIX-socket
	... create a PKT that contains debug-information
	... forward the DBG-packets
	... send immidiately a ACK with debug-informations if ARET is set
	... destination sends ACK with debug-informations
	... the mac-address of a node is add to ACK if INSP is set
	... stores an ACK, that reachs its destination, as a string in a queue.

Not all of the implemented functions have been tested so far. Nevertheless with the command

	echo "WRITE debug_handler.ping 192.168.56.101|192.168.56.102|1|0|0|0" | nc -U /tmp/socket 

a PKT with the DBG flag set will be send from 192.168.56.101 to 192.168.56.102. 
This node will return an ACK, also with an DBG flag set. The ACK will be saved (with a timestamp) 
as a string in a queue. With the command

	echo "READ debug_handler.ping" | nc -U /tmp/socket

the string can be read and removed from the queue.  

An example is shown in the screenshots pc1_castor.png, pc1_ping.png and pc2_castor.png

---------------------------------------------------------------------------------------------------

Click/Castor:
=============

Modules:
	CastorDebugHandler: 		
		Implements the read/write handler of Click. From a received string a PKT is created 
		and incoming ACKs will be saved in a queue.
	CastorCreateDebugPkt:		
		Reiceives the created packet from the CastorDebugHandler and 
		adds some more informations like flowid and pid.
		(Der Klassenname stimmt eigentlich nicht wirklich, wahrscheinlich
		kann man die Funktionalität auch vollständig in der
		Handlerklasse übernehmen. (wird noch geändert))
	CastorHandlePkt:
		CastorForwardPkt:	
			Has been modified so that an debug-packet is not authenticated and
			the CastorCreateDebugAck module is executed if ARET is set.
			
			CastorCreateDebugAck:	
				Creates an ACK. If the INSP flag was set in the PKT, the mac-address
				of this node will be added to the ACK. The PKT is also forwarded.
		CastorLocalPkt:		
			At the moment this module only creates an ACK with CastorCreateDebugAck,
			for a debug-packet, if the ARET is set and forwards both packetes.
	CastorHandleAck:
		ACKs will be checked with CastorDebugAuthenticateAck if the DBG flag was set.
		If also INSP is set the mac-address of the node will be added. Is the node the
		original source, the packet will be send to the CastorDebugHandler.
		
		CastorDebugAuthenticateAck:	
			Checks merely, if the corresponding PKT of the received ACK was forwarded.
		CastorInsertPath:
			Adds the mac-address of the current node to the ACK.
		CastorDebugNoLoopback:
			Either the ACK is forwarded or it matches with a PKT that this node has 
			send earlier, in this case the packet is forwarded to the 
			CastorDebugHandler.

All the new modules are shown in the diagram.pdf

----------------------------------------------------------------------------------------------------

PING:
=====

The implementation of Ping currently contains the following functionalities:
	- Accepts all options named in the software design. This is done by the CLI-class.
	- Reads the ip-address of the right network interface.
	- Connects to the UNIX-socket provided by Click
	- If a PKT packet is to be send, the information 
	  "<src_ip>|<dst_ip>|DBG|ARET|INSP|size|" are written to the socket.
	- The number of preloaded packets is send before trying to read the first ACK. 
	- The informations of a received ACK are printed.
	- At the end all informations will be analyzed like described in the software design.


The main functionality is implemented, but there are still problems with the method that should 
read/write the strings to the UNIX-socket. After write() sends the string with all information to the
socket, read() receives the answer of the socket line by line. After the last line is read this 
function doesn't return zero like expected and waits. This problem will be fixed as soon as possible, 
but up to this time the implementation can't be tested with all functionalities. An interrupt handler 
is also missing and will be added. 


Traceroute:
===========

The implementation of Traceroute will be done in the next iteration (Beta Version).
===========

The implementation of Traceroute will be done in the next iteration (Beta Version). 
