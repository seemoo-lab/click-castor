/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// Adaptation of examples/udp/udp-client-server.cc for
// Click based nodes running wifi.
//
// Network topology:
//
//               (1.4)
//             (( n4 ))
//
//          172.16.1.0/24
//
//   (1.1)      (1.2)       (1.3)
//     n0 ))   (( n1 ))   (( n2
//               WLAN
//
// - UDP flows from n0 to n1 and n2 to n1.
// - All nodes are Click based.
// - The single ethernet interface that each node
//   uses is named 'eth0' in the Click file.

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-click-routing.h"
#include "ns3/click-internet-stack-helper.h"
#include "ns3/random-variable-stream.h"
//#include "ns3/log.h"
//#include "ns3/object.h"

using namespace ns3;

#ifndef NS3_CLICK
#define NS3_CLICK
#endif

NS_LOG_COMPONENT_DEFINE ("NsclickCastor");

#ifdef NS3_CLICK
void
ReadArp (Ptr<Ipv4ClickRouting> clickRouter)
{
  // Access the handlers
  NS_LOG_INFO (clickRouter->ReadHandler ("arpquerier", "table"));
  NS_LOG_INFO (clickRouter->ReadHandler ("arpquerier", "stats"));
}

void
WriteArp (Ptr<Ipv4ClickRouting> clickRouter, size_t nNodes, const Ipv4Address& base)
{
	// Access the handler
	for(unsigned int i = 1; i <= nNodes; i++) {
		Ipv4Address ip = Ipv4Address(base.Get() + i);
		Mac48Address mac;
		uint8_t buf[6] = { 0 };
		buf[5] = i;
		mac.CopyFrom(buf);
		std::stringstream stream;
		ip.Print(stream);
		stream << " " << mac;
//		ip.Print(std::cout);
//		std::cout << " " << mac << "\n";
		clickRouter->WriteHandler("arpquerier", "insert", stream.str().c_str());
	}
	// TODO: Deal with entry expiry -> set appropriate arpquerier parameter
	// TODO: Currently only works with 255 nodes
}
#endif

/**
 * Returns random value between 0 and max (both inclusive)
 */
inline double getRand(double max) {
	double min = 0.0;
	Ptr<UniformRandomVariable> randVar = CreateObject<UniformRandomVariable> ();
	randVar->SetAttribute ("Min", DoubleValue (min));
	randVar->SetAttribute ("Max", DoubleValue (max));

	return randVar->GetValue();
}

int
main (int argc, char *argv[])
{
#ifdef NS3_CLICK
	// New seed on every run
	//RngSeedManager::SetSeed(time(0));

	SeedManager::SetSeed (100);
	SeedManager::SetRun (1);

	// Simulation parameters
	const double xSize = 200.0;
	const double ySize = 200.0;
	const double transmissionRange = 100.0;

	const size_t nNodes = 10;

	const Ipv4Address baseAddr("192.168.201.0");

  //
  // Enable logging
  //
  LogComponentEnable ("NsclickCastor", LOG_LEVEL_INFO);

  //
  // Explicitly create the nodes required by the topology (shown above).
  //
  NS_LOG_INFO ("Create nodes.");
  NodeContainer n;
  n.Create (nNodes);

  NS_LOG_INFO ("Create channels.");
  //
  // Explicitly create the channels required by the topology (shown above).
  //
  std::string phyMode ("DsssRate11Mbps");

  // disable fragmentation for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
  // turn off RTS/CTS for frames below 2200 bytes
    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
  // Fix non-unicast data rate to be the same as that of unicast
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",
                      StringValue (phyMode));

  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);

  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  // This is one parameter that matters when using FixedRssLossModel
  // set it to zero; otherwise, gain will be added
//  wifiPhy.Set ("RxGain", DoubleValue (0) );
  // ns-3 supports RadioTap and Prism tracing extensions for 802.11b
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel", "Speed", DoubleValue(299792458.0/100));
//  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  // The below FixedRssLossModel will cause the rss to be fixed regardless
  // of the distance between the two stations, and the transmit power
//  wifiChannel.AddPropagationLoss ("ns3::FixedRssLossModel","Rss",DoubleValue (-80));
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel","MaxRange",DoubleValue (transmissionRange));
  //wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
  wifiPhy.SetChannel (wifiChannel.Create ());

  // Add a non-QoS upper mac, and disable rate control
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));
  // Set it to adhoc mode
  wifiMac.SetType ("ns3::AdhocWifiMac");
  NetDeviceContainer d = wifi.Install (wifiPhy, wifiMac, n);

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for(unsigned int i = 0; i < nNodes; i++) {
//	  double x = xSize / nNodes * i;
//	  double y = 0 * ySize;
	  double x = getRand(xSize);
	  double y = getRand(ySize);
	  char buf[100];
	  sprintf(buf, "Placing node %d on (%.1f,%.1f)", i, x, y);
	  NS_LOG_INFO(buf);
	  positionAlloc->Add(Vector(x,y,0));
  }
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (n);

  //
  // Install Click on the nodes
  //
  ClickInternetStackHelper clickinternet;
  for(unsigned int i = 0; i < n.GetN(); i++) {
	  clickinternet.SetClickFile (n.Get (i), "/home/milan/click/conf/castor/castor_unicast_routing.click");
  }
  clickinternet.SetRoutingTableElement (n, "rt");
  clickinternet.Install (n);
  Ipv4AddressHelper ipv4;

  //
  // We've got the "hardware" in place.  Now we need to add IP addresses.
  //
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase (baseAddr, Ipv4Mask("255.255.255.0"));
  Ipv4InterfaceContainer i = ipv4.Assign (d);

  NS_LOG_INFO ("Create Applications.");
  //
  // Create one udpServer applications on node one.
  //
  uint16_t port = 4000;
  UdpServerHelper server (port);
  ApplicationContainer apps = server.Install (n.Get (n.GetN() - 1));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (11.0));

  //
  // Create one UdpClient application to send UDP datagrams from node zero to
  // node one.
  //
  uint32_t MaxPacketSize = 1024;
  Time interPacketInterval = Seconds (1);
  uint32_t maxPacketCount = 320;

  UdpClientHelper client (i.GetAddress (n.GetN() - 1), port);
  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  apps = client.Install (NodeContainer (n.Get (0)));
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (5.0));

  wifiPhy.EnablePcap ("nsclick-udp-client-server-wifi", d);

  // Force the MAC address of the second node: The current ARP
  // implementation of Click sends only one ARP request per incoming
  // packet for an unknown destination and does not retransmit if no
  // response is received. With the scenario of this example, all ARP
  // requests of node 3 are lost due to interference from node
  // 1. Hence, we fill in the ARP table of node 2 before at the
  // beginning of the simulation
    for(unsigned int i = 0; i < n.GetN(); i++) {
  	  Simulator::Schedule (Seconds (0.5), &WriteArp, n.Get (i)->GetObject<Ipv4ClickRouting> (), nNodes, baseAddr);
    }

  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (20.0));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
#else
  NS_FATAL_ERROR ("Can't use ns-3-click without NSCLICK compiled in");
#endif
}
