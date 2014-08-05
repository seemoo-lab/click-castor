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

using namespace ns3;

#ifndef NS3_CLICK
#define NS3_CLICK
#endif

NS_LOG_COMPONENT_DEFINE("NsclickCastor");

#ifdef NS3_CLICK

void WriteArp(Ptr<Ipv4ClickRouting> clickRouter, size_t nNodes,	const Ipv4Address& base) {
	// Access the handler
	for (unsigned int i = 1; i <= nNodes; i++) {
		Ipv4Address ip = Ipv4Address(base.Get() + i);
		Mac48Address mac;
		uint8_t buf[6] = { 0 };
		buf[5] = i;
		mac.CopyFrom(buf);

		// Create entry of form "<IP> <MAC>"
		std::stringstream stream;
		ip.Print(stream);
		stream << " " << mac;

		clickRouter->WriteHandler("arpquerier", "insert", stream.str().c_str());
	}
	// TODO: Currently only works with 254 nodes
}

void WriteXcastMap(Ptr<Ipv4ClickRouting> clickRouter, Ipv4Address group, const std::vector<Ipv4Address>& destinations) {
	// Create entry of form "<GroupAddr> <Dest1Addr> <Dest2Addr> ... <DestNAddr>"
	std::stringstream stream;

	// Multicast address is first in list
	group.Print(stream);

	for (unsigned int i = 0; i < destinations.size(); i++) {
		stream << " ";
		destinations[i].Print(stream);
	}

	clickRouter->WriteHandler("handleIpPacket/map", "insert", stream.str().c_str());
}

/**
 * Returns random value between 0 and max (both inclusive)
 */
inline double getRand(double max) {
	double min = 0.0;
	Ptr<UniformRandomVariable> randVar = CreateObject<UniformRandomVariable>();
	randVar->SetAttribute("Min", DoubleValue(min));
	randVar->SetAttribute("Max", DoubleValue(max));

	return randVar->GetValue();
}

// has to be defined here, cannot be used as local template argument
struct LessIpv4Address {
	bool operator()(const Ipv4Address& x, const Ipv4Address& y) const {
		return x.Get() < y.Get();
	}
};

#endif

int main(int argc, char *argv[]) {
#ifdef NS3_CLICK

	RngSeedManager::SetSeed(100);
	RngSeedManager::SetRun(1);

	// Simulation parameters / topology
	const double xSize = 3000.0;
	const double ySize = 3000.0;
	const double transmissionRange = 500.0;
	const size_t nNodes = 100;

	// number of senders/flows/groups
	const size_t nSenders = 1;
	const size_t groupSize = 2;

	uint32_t MaxPacketSize = 256;


	const Time duration = Seconds(2.0);

	// Network
	const Ipv4Address baseAddr("192.168.201.0");
	const Ipv4Mask networkMask("255.255.255.0");
	const Ipv4Address groupAddr("224.0.2.0");

	// Setup groups

	std::map<Ipv4Address, Ipv4Address, LessIpv4Address> senderGroupAssign;
	std::map<Ipv4Address, std::vector<Ipv4Address>, LessIpv4Address> groups; // multicast group -> xcast receivers

	for (unsigned int i = 0, iAddr = 1; i < nSenders; i++) {
		Ipv4Address sender = Ipv4Address(baseAddr.Get() + iAddr);
		Ipv4Address group = Ipv4Address(groupAddr.Get() + iAddr);
		std::vector<Ipv4Address> xcastDestinations;
		iAddr++;
		for (unsigned int j = 0; j < groupSize; j++, iAddr++) {
			Ipv4Address dest = Ipv4Address(baseAddr.Get() + iAddr);
			xcastDestinations.push_back(dest);
		}
		senderGroupAssign.insert(std::make_pair(sender, group));
		groups.insert(std::make_pair(group, xcastDestinations));
	}

	//
	// Enable logging
	//
	LogComponentEnable("NsclickCastor", LOG_LEVEL_INFO);

	//
	// Explicitly create the nodes required by the topology (shown above).
	//
	NS_LOG_INFO("Create nodes.");
	NodeContainer n;
	n.Create(nNodes);

	NS_LOG_INFO("Create channels.");
	//
	// Explicitly create the channels required by the topology (shown above).
	//
	std::string phyMode("DsssRate1Mbps");

	// disable fragmentation for frames below 2200 bytes
	Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("2200"));
	// turn off RTS/CTS for frames below 2200 bytes
	Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
	// Fix non-unicast data rate to be the same as that of unicast
	Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode",	StringValue(phyMode));

	WifiHelper wifi;
	wifi.SetStandard(WIFI_PHY_STANDARD_80211b);

	YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
	// This is one parameter that matters when using FixedRssLossModel
	// set it to zero; otherwise, gain will be added
//  wifiPhy.Set ("RxGain", DoubleValue (0) );
	// ns-3 supports RadioTap and Prism tracing extensions for 802.11b
	wifiPhy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

	YansWifiChannelHelper wifiChannel;
	//wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel", "Speed", DoubleValue(299792458.0));
	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	// The below FixedRssLossModel will cause the rss to be fixed regardless
	// of the distance between the two stations, and the transmit power
	wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(transmissionRange));
	//wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
	wifiPhy.SetChannel(wifiChannel.Create());

	// Add a non-QoS upper mac, and disable rate control
	NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default();
	wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
			StringValue(phyMode), "ControlMode", StringValue(phyMode));
	// Set it to adhoc mode
	wifiMac.SetType("ns3::AdhocWifiMac");
	NetDeviceContainer d = wifi.Install(wifiPhy, wifiMac, n);

	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<
			ListPositionAllocator>();
	for (unsigned int i = 0; i < nNodes; i++) {
		double x = getRand(xSize);
		double y = getRand(ySize);
		positionAlloc->Add(Vector(x, y, 0));
	}
	mobility.SetPositionAllocator(positionAlloc);
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.Install(n);

	//
	// Install Click on the nodes
	//
	ClickInternetStackHelper clickinternet;
	for (unsigned int i = 0; i < n.GetN(); i++)
		clickinternet.SetClickFile(n.Get(i), "/home/milan/click/conf/castor/castor_xcast_routing.click");
	clickinternet.SetRoutingTableElement(n, "rt");
	clickinternet.Install(n);
	Ipv4AddressHelper ipv4;

	//
	// We've got the "hardware" in place.  Now we need to add IP addresses.
	//
	NS_LOG_INFO("Assign IP Addresses.");
	ipv4.SetBase(baseAddr, networkMask);
	Ipv4InterfaceContainer i = ipv4.Assign(d);

	NS_LOG_INFO("Create Applications.");
	//
	// Create one udpServer applications on node one.
	//
	// TODO Install servers on all destinations
	uint16_t port = 4000;
	UdpServerHelper server(port);
	ApplicationContainer apps = server.Install(n.Get(n.GetN() - 1));
	apps.Start(Seconds(1.0));
	apps.Stop(duration + Seconds(3.0));

	//
	// Create one UdpClient application to send UDP datagrams from node zero to
	// node one.
	//
	Time interPacketInterval = Seconds(0.25);
	uint32_t maxPacketCount = UINT32_MAX;

	for (unsigned int i = 1; i <= nSenders; i++) {
		Ipv4Address ip = Ipv4Address(groupAddr.Get() + i);
		UdpClientHelper client(ip, port);
		client.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
		client.SetAttribute("Interval", TimeValue(interPacketInterval));
		client.SetAttribute("PacketSize", UintegerValue(MaxPacketSize));
		apps = client.Install(NodeContainer(n.Get(i - 1)));
		apps.Start(Seconds(2.0) + interPacketInterval / nSenders * (i - 1));
		apps.Stop(duration + Seconds(2.0));
	}

	//wifiPhy.EnablePcap("castor-xcast", d);

	// Force the MAC address of the second node: The current ARP
	// implementation of Click sends only one ARP request per incoming
	// packet for an unknown destination and does not retransmit if no
	// response is received. With the scenario of this example, all ARP
	// requests of node 3 are lost due to interference from node
	// 1. Hence, we fill in the ARP table of node 2 before at the
	// beginning of the simulation
	for (unsigned int i = 0; i < n.GetN(); i++) {
		Simulator::Schedule(Seconds(0.5), &WriteArp, n.Get(i)->GetObject<Ipv4ClickRouting>(), nNodes, baseAddr);
		// Write Xcast destination mapping
		for (std::map<Ipv4Address, std::vector<Ipv4Address> >::iterator it = groups.begin(); it != groups.end(); it++)
			Simulator::Schedule(Seconds(0.5), &WriteXcastMap, n.Get(i)->GetObject<Ipv4ClickRouting>(), it->first, it->second);
	}

	//
	// Now, do the actual simulation.
	//
	NS_LOG_INFO("Run Simulation.");
	Simulator::Stop(duration + Seconds(4.0));
	Simulator::Run();
	Simulator::Destroy();
	NS_LOG_INFO("Done.");
#else
	NS_FATAL_ERROR ("Can't use ns-3-click without NSCLICK compiled in");
#endif
}
