/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Paint frames that were broadcasted by the sender
 */
elementclass BroadcastPainter {
	input[0]
		-> broadcastFilter :: HostEtherFilter(FF:FF:FF:FF:FF:FF)
		-> Paint(10, ANNO 38) // Packet was broadcast to us
		-> output;

	broadcastFilter[1]
		-> output; // Packet was unicasted to us (no paint)
}

elementclass CastorClassifier {
	$myAddr, $neighbors |

	input
		-> annoSrcAddr :: GetEtherAddress(ANNO 0, OFFSET src)
		-> neighborFilter :: NeighborFilter()
		-> AddNeighbor($neighbors, ENABLE $neighborsEnable) // add all neighbors that we receive transmissions from
		-> BroadcastPainter
		-> ethclassifier :: Classifier(12/88B6, 12/88B5, -) // (0) Castor PKT/ACK; (1) beacon; (2) other
		-> removeEthernetHeader :: Strip(14)
		-> forwarderFilter :: ForwarderFilter($myAddr)
		-> NeighborAuthCheckICV(crypto, $neighborsEnable)
		=> ( [0] -> NeighborAuthStripICV -> output;
		     [1] /* -> CastorPrint("No valid neighbor-to-neighbor ICV (normal at sender of relayed broadcast messages)", fake) */ -> Discard; )
		-> removeForwarderList :: RemoveForwarderList
		-> cclassifier :: Classifier(0/c?, 0/a?, -);

	cclassifier[0] // Castor PKTs -> output 0
		-> [0]output;

	cclassifier[1] // Castor ACKs -> output 1
		-> [1]output;

	cclassifier[2] // Malformed Castor packet
		-> Print("[DROP] malformed Castor packet")
		-> Discard;

	ethclassifier[1] // Beacon
		// Already updated Neighbor table
		-> Discard

	ethclassifier[2] // other
		//-> Print("[DROP] non-Castor packet")
		-> Discard;
}

elementclass DynamicEtherEncap {
	$myAddr, $neighbors, $crypto |

	input
		-> AddForwarderList
		-> NeighborAuthAddICV($neighbors, $crypto, true)
		=> ( [0] -> output;
		     [1] -> RemoveForwarderList -> CastorPrint("No neighbors, no ICV added", fake) -> Discard; )
		-> EtherEncap($ETHERTYPE_CASTOR, $myAddr, 00:00:00:00:00:00)
		-> StoreEtherAddress(OFFSET dst, ANNO 12)
		-> output;
}

elementclass CastorHandleAck {
	$myIP, $routingtable, $flowtable, $continuousflow, $timeouttable, $ratelimits, $history, $neighbors, $crypto |

	// Regular ACK flow
	input
		-> calcPid :: CastorAnnotatePid($crypto)
		-> AddReplayAck(replaystore)
		-> authenticate :: CastorAuthenticateAck($history)
		-> updateTimeout :: CastorUpdateTimeout($timeouttable, $history, VERBOSE false)
		-> updateEstimates :: CastorUpdateEstimates($routingtable, $continuousflow, $history, $copyEstimators, $useFirstAck)
		-> CastorAddAckToHistory($history, $flowtable)
		//-> CastorPrint('Received valid', $myIP)
		-> noLoopback :: CastorNoLoopback($history, $myIP)
		// It does not make sense to update the rate limit packets that I sent myself
		-> updateRate :: CastorUpdateRateLimit($ratelimitEnable, $ratelimits, $history)
		-> setAckNexthop :: CastorSetAckNexthop($history, $neighbors)
		//-> recAck :: CastorRecordPkt
		-> output;

	// Discarding...
	null :: Discard;
	authenticate[1]
		//-> CastorPrint("Unknown corresponding PKT", $myIP)
		-> null;
	authenticate[2]
		// Might rarely happen if MAC ACK was lost and Castor ACK is retransmitted
		//-> CastorPrint("ACK duplicate from same neighbor", $myIP)
		-> null;
	authenticate[3]
		//-> CastorPrint("ACK from different neighbor than PKT was forwarded to", $myIP)
		-> null;
	updateEstimates[1]
		//-> CastorPrint("Duplicate, add to history", $myIP)
		-> CastorAddAckToHistory($history, $flowtable)
		-> null;
	noLoopback[1]
		//-> CastorPrint("Don't send to myself", $myIP)
		-> null;
}

elementclass CastorBlackhole {
	input
		-> filter :: CastorUnicastFilter
		-> output;

	filter[1]
		//-> rec :: CastorRecordPkt
		-> [1]output;
}

/**
 * Creates IP unicast from multicast packets
 */
elementclass CastorHandleIpPacket {
	$myAddrInfo, $flowmanager, $flowtable, $crypto |

	input
	-> CastorAddHeader($flowmanager, $flowtable, $forceNonce)
	-> CastorCalcICV($crypto)
	//-> CastorPrint('Send', $myAddrInfo)
	//-> rec :: CastorRecordPkt
	-> output;
}

elementclass CastorLocalPkt {
	$myIP, $flowtable, $timeouttable, $history, $crypto |

	input
		//-> CastorPrint('Packet reached destination', $myIP)
		-> CastorStripFlowAuthenticator
		// Check ICV before trying to authenticate flow
		-> auth :: CastorCheckICV($crypto)
		-> CastorIsSync[0,1]
		=> (input[0]
				-> CastorHasCompleteFlow($flowtable)[0,1]
				=> (input[0] -> output;
					input[1] -> CastorPrint("SYN missing", $myIP) -> Discard;)
				-> output;
			// this is only done once per flow
			input[1] -> CastorReconstructFlow($flowtable, $crypto) -> output;)
		-> authFlow :: CastorAuthenticateFlow($flowtable, $crypto)
		-> addPktToHistory :: CastorAddPktToHistory($history)
		-> CastorStartTimer($timeouttable, $history, FLOW_TABLE $flowtable, ID $myIP, VERBOSE false)
		//-> rec :: CastorRecordPkt
		-> genAck :: CastorCreateAck($flowtable)
		-> [0]output;

	genAck[1] // Generate ACK for received PKT
		//-> CastorPrint('Generated', $myIP)
		-> calcPid :: CastorAnnotatePid($crypto)
		-> CastorAddAckToHistory($history, $flowtable)
		-> [1]output; // Push ACKs to output 1

	// If invalid -> discard
	null :: Discard;
	authFlow[1]
		-> CastorPrint("Flow authentication failed", $myIP)
		-> null;
	auth[1]
		-> CastorPrint("Invalid ICV", $myIP)
		-> null;
}

elementclass CastorForwardPkt {
	$myIP, $routeselector, $routingtable, $flowtable, $timeouttable, $ratelimits, $history, $crypto |

	input
		-> auth :: CastorAuthenticateFlow($flowtable, $crypto)
		-> AddReplayPkt(replaystore)
		-> blackhole :: CastorBlackhole // inactive by default
		-> route :: CastorLookupRoute($routeselector)
		-> CastorAddPktToHistory($history)
		-> CastorStartTimer($timeouttable, $history, $routingtable, $flowtable, $ratelimits, ID $myIP, VERBOSE false)
		-> addFlowAuthenticator :: CastorAddFlowAuthenticator($flowtable, $fullFlowAuth)
		//-> CastorPrint('Forwarding Packet', $myIP)
		//-> rec :: CastorRecordPkt
		-> [0]output;

	// Bounce back to sender
	auth[2]
		-> CastorPrint("Flow authentication failed (insufficient flow auth elements)", $myIP)
		-> Discard;

	auth[1]
		-> CastorPrint("Flow authentication failed", $myIP)
		-> Discard;

	route[1]
		-> CastorPrint("No suitable PKT forwarding contact", $myIP)
		-> Discard;

	blackhole[1]
	    -> blackholeExceptions :: NeighborFilter(ACTIVE true)
	    -> route
}

/**
 * Input:	Castor PKT
 * Output(0):	PKT for local host
 * Output(1):	New ACK
 * Output(2):	Forwarded PKT
 */
elementclass CastorHandlePkt {
	$myIP, $routeselector, $routingtable, $flowtable, $timeouttable, $ratelimits, $history, $crypto |

	input
		-> log :: CastorLogPkt()
		-> checkDuplicate :: CastorCheckDuplicate($history, $flowtable, $replayProtect)
		-> destinationClassifier :: CastorDestClassifier($myIP);

 	// PKT arrived at destination
	destinationClassifier[0]
		-> handleLocal :: CastorLocalPkt($myIP, $flowtable, $timeouttable, $history, $crypto)
		-> [0]output;

	// PKT needs to be forwarded
	destinationClassifier[1]
		-> forward :: CastorForwardPkt($myIP, $routeselector, $routingtable, $flowtable, $timeouttable, $ratelimits, $history, $crypto)
		-> [2]output;

	handleLocal[1]
		//-> recAck :: CastorRecordPkt
		-> [1]output;

	// Need to retransmit ACK
	checkDuplicate[1]
		-> CastorAddPktToHistory($history)
		-> CastorRetransmitAck($history)
		-> noLoopback :: CastorNoLoopback($history, $myIP) // The src node should not retransmit ACKs
		//-> CastorPrint("Duplicate pid, retransmit ACK", $myIP)
		//-> recAck;
		-> [1]output;

	// If invalid or duplicate -> discard
	null :: Discard;
	checkDuplicate[2]
		//-> CastorPrint("PKT duplicate, but from different neighbor", $myIP)
		-> CastorAddPktToHistory($history) // Add sender to history
		-> null;

	checkDuplicate[3]
		// Might rarely happen if MAC ACK was lost and Castor PKT is retransmitted
		//-> CastorPrint("PKT duplicate from same neighbor", $myIP)
		-> null;

	noLoopback[1]
		//-> CastorPrint("Source trying to retransmit", $myIP)
		-> null;

}
