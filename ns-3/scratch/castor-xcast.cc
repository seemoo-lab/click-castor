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

unsigned int readIntStat(Ptr<Ipv4ClickRouting> clickRouter, std::string what, std::string where) {
	std::string result = clickRouter->ReadHandler(where, what);
	unsigned int i;
	sscanf(result.c_str(), "%d", &i);
	return i;
}

unsigned int readPidCount(Ptr<Ipv4ClickRouting> clickRouter, std::string where) {
	return readIntStat(clickRouter, "num", where);
}

unsigned int readPktCount(Ptr<Ipv4ClickRouting> clickRouter, std::string where) {
	return readIntStat(clickRouter, "numUnique", where);
}

unsigned int readAccumPktSize(Ptr<Ipv4ClickRouting> clickRouter, std::string where) {
	return readIntStat(clickRouter, "size", where);
}

unsigned int readBroadcasts(Ptr<Ipv4ClickRouting> clickRouter, std::string where) {
	return readIntStat(clickRouter, "broadcasts", where);
}

unsigned int readUnicasts(Ptr<Ipv4ClickRouting> clickRouter, std::string where) {
	return readIntStat(clickRouter, "unicasts", where);
}

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

NetDeviceContainer setPhysicalChannel(NodeContainer& nodes, double transmissionRange) {
	std::string phyMode("DsssRate11Mbps");

	// disable fragmentation for frames below 2200 bytes
	Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("2200"));
	// turn off RTS/CTS for frames below 2200 bytes
	Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
	// Fix non-unicast data rate to be the same as that of unicast
	Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode",	StringValue(phyMode));

	WifiHelper wifi;
	wifi.SetStandard(WIFI_PHY_STANDARD_80211b);

	YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
	// ns-3 supports RadioTap and Prism tracing extensions for 802.11b
	wifiPhy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel", "Speed", DoubleValue(299792458.0));
	//wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

	wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(transmissionRange));
	//wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
	wifiPhy.SetChannel(wifiChannel.Create());

	// Add a non-QoS upper mac, and disable rate control
	NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default();
	wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
			StringValue(phyMode), "ControlMode", StringValue(phyMode));
	// Set it to adhoc mode
	wifiMac.SetType("ns3::AdhocWifiMac");
	return wifi.Install(wifiPhy, wifiMac, nodes);
}

Ptr <PositionAllocator> getRandomRectanglePositionAllocator(double xSize, double ySize) {
	ObjectFactory pos;
	pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
	std::ostringstream xSizeUniformRandomVariable;
	xSizeUniformRandomVariable << "ns3::UniformRandomVariable[Min=0.0|Max=" << xSize << "]";
	pos.Set("X", StringValue(xSizeUniformRandomVariable.str()));
	std::ostringstream ySizeUniformRandomVariable;
	ySizeUniformRandomVariable << "ns3::UniformRandomVariable[Min=0.0|Max=" << ySize << "]";
	pos.Set("Y", StringValue(ySizeUniformRandomVariable.str()));
	return pos.Create ()->GetObject <PositionAllocator> ();
}

void setRandomWaypointMobility(NodeContainer& nodes, double xSize, double ySize, double speed, double pause) {
	MobilityHelper mobility;
	Ptr <PositionAllocator> taPositionAlloc = getRandomRectanglePositionAllocator(xSize, ySize);

	std::ostringstream speedVariable;
	speedVariable << "ns3::UniformRandomVariable[Min=0.0|Max="<< speed << "]";
	std::ostringstream pauseVariable;
	pauseVariable << "ns3::UniformRandomVariable[Min=0.0|Max=" << pause << "]";

	mobility.SetMobilityModel("ns3::RandomWaypointMobilityModel",
			"Speed", StringValue(speedVariable.str()),
			"Pause", StringValue(pauseVariable.str()),
			"PositionAllocator", PointerValue(taPositionAlloc));
	mobility.SetPositionAllocator(taPositionAlloc);
	mobility.Install (nodes);
}

void setConstantPositionMobility(NodeContainer& nodes, double xSize, double ySize) {
	MobilityHelper mobility;
	Ptr <PositionAllocator> taPositionAlloc = getRandomRectanglePositionAllocator(xSize, ySize);
	mobility.SetPositionAllocator(taPositionAlloc);
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.Install(nodes);
}

void setClickRouter(NodeContainer& nodes, StringValue clickConfig) {
	ClickInternetStackHelper clickinternet;
	for (unsigned int i = 0; i < nodes.GetN(); i++)
		clickinternet.SetClickFile(nodes.Get(i), clickConfig.Get());
	clickinternet.SetRoutingTableElement(nodes, "rt");
	clickinternet.Install(nodes);
}

#endif

int main(int argc, char *argv[]) {
#ifdef NS3_CLICK
	//
	// Enable logging
	//
	LogComponentEnable("NsclickCastor", LOG_LEVEL_INFO);

	CommandLine cmd;
	cmd.Parse (argc, argv);

	RngSeedManager::SetSeed(12345);
	RngSeedManager::SetRun(4);

	// Simulation parameters / topology
	const double xSize = 1000.0;
	const double ySize = 1000.0;
	const double transmissionRange = 550.0;
	const size_t nNodes = 10;

	const double speed = 0.0;
	const double pause = 60.0;

	// number of senders/flows/groups
	const size_t nSenders = 2;
	const size_t groupSize = 5;

	uint32_t MaxPacketSize = 256 - 28; // IP+UDP header size = 28 byte

	Time interPacketInterval = Seconds(0.25);

	const Time duration = Seconds(30.0);

	const StringValue clickConfig("/home/milan/click/conf/castor/castor_xcast_routing.click");
//	const StringValue clickConfig("/home/milan/click/conf/castor/castor_multicast_via_unicast_routing.click");

	// Network
	const Ipv4Address baseAddr("192.168.201.0");
	const Ipv4Mask networkMask("255.255.255.0");
	const Ipv4Address groupAddr("224.0.2.0");

	// Setup groups based on 'nSenders' (e.g. 2) and 'groupSize' (e.g. 3) in the format:
	// 'senderGroupAssign':
	// 		192.168.201.1 -> 224.0.2.1; 192.168.201.5 -> 224.0.2.5
	std::map<Ipv4Address, Ipv4Address, LessIpv4Address> senderGroupAssign;
	// 'groups':
	// 		192.168.201.1 -> 192.168.201.2, 192.168.201.3, 192.168.201.4;
	// 		192.168.201.5 -> 192.168.201.6, 192.168.201.7, 192.168.201.8
	std::map<Ipv4Address, std::vector<Ipv4Address>, LessIpv4Address> groups; // multicast group -> xcast receivers

	for (unsigned int i = 0, iAddr = 1; i < nSenders; i++) {
		Ipv4Address sender = Ipv4Address(baseAddr.Get() + iAddr);
		Ipv4Address group = Ipv4Address(groupAddr.Get() + iAddr);
		std::vector<Ipv4Address> xcastDestinations;
		iAddr = iAddr % nNodes + 1; // Circular count from 1..nNodes
		for (unsigned int j = 0; j < groupSize; j++, iAddr = iAddr % nNodes + 1) {
			Ipv4Address dest = Ipv4Address(baseAddr.Get() + iAddr);
			xcastDestinations.push_back(dest);
		}
		senderGroupAssign.insert(std::make_pair(sender, group));
		groups.insert(std::make_pair(group, xcastDestinations));
	}

	//
	// Explicitly create the nodes required by the topology (shown above).
	//
	NS_LOG_INFO("Create nodes.");
	NodeContainer n;
	n.Create(nNodes);

	NS_LOG_INFO("Create channels.");
	NetDeviceContainer d = setPhysicalChannel(n, transmissionRange);
//	setConstantPositionMobility(n, xSize, ySize);
	setRandomWaypointMobility(n, xSize, ySize, speed, pause);

	NS_LOG_INFO("Setup Click Routers");
	setClickRouter(n, clickConfig);

	NS_LOG_INFO("Assign IP addresses.");
	Ipv4AddressHelper ipv4;
	ipv4.SetBase(baseAddr, networkMask);
	Ipv4InterfaceContainer i = ipv4.Assign(d);

	NS_LOG_INFO("Create Applications.");
	ApplicationContainer apps;
	uint16_t port = 4000;
	uint32_t maxPacketCount = UINT32_MAX;
	unsigned int nodeIndex = 0;
	for (std::map<Ipv4Address, Ipv4Address>::iterator it = senderGroupAssign.begin(); it != senderGroupAssign.end(); it++) {
		Ipv4Address groupIp = it->second;

		// Setup multicast source
		UdpClientHelper client(groupIp, port);
		client.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
		client.SetAttribute("Interval", TimeValue(interPacketInterval));
		client.SetAttribute("PacketSize", UintegerValue(MaxPacketSize));
		apps = client.Install(NodeContainer(n.Get(nodeIndex)));
		apps.Start(Seconds(2.0) + interPacketInterval / nSenders * nodeIndex);
		apps.Stop(duration + Seconds(2.0));

		nodeIndex++;

		std::vector<Ipv4Address> dsts = groups.at(groupIp);
		for(std::vector<Ipv4Address>::iterator itDst = dsts.begin(); itDst != dsts.end(); itDst++) {
			UdpServerHelper server(port);
			apps = server.Install(n.Get(nodeIndex));
			apps.Start(Seconds(1.0));
			apps.Stop(duration + Seconds(3.0));
		}
	}

	//wifiPhy.EnablePcap("castor-xcast", d);

	// We fill in the ARP tables at the beginning of the simulation
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
	time_t start; time(&start);
	Simulator::Stop(duration + Seconds(6.0));
	Simulator::Run();
	time_t end; time(&end);
	NS_LOG_INFO("Done after " << difftime(end, start) << " seconds");

	//
	// Schedule evaluation
	//
	uint32_t numPidsSent = 0;
	uint32_t numPidsRecv = 0;
	uint32_t bandwidthUsage = 0;
	uint32_t numPktsSent = 0;
	uint32_t broadcasts = 0;
	uint32_t unicasts = 0;
	uint32_t numPktsForwarded = 0;

	std::string forwarding = "handlepkt/forward/rec";
	std::string sending = "handleIpPacket/rec";
	std::string delivering = "handlepkt/handleLocal/rec";
	for(unsigned int i = 0; i < nNodes; i++) {
		numPidsSent += readPidCount(n.Get(i)->GetObject<Ipv4ClickRouting>(), sending);
		numPidsRecv += readPidCount(n.Get(i)->GetObject<Ipv4ClickRouting>(), delivering);
		bandwidthUsage += readAccumPktSize(n.Get(i)->GetObject<Ipv4ClickRouting>(), forwarding);
		numPktsSent += readPktCount(n.Get(i)->GetObject<Ipv4ClickRouting>(), sending);
		numPktsForwarded += readPktCount(n.Get(i)->GetObject<Ipv4ClickRouting>(), forwarding);
		broadcasts += readBroadcasts(n.Get(i)->GetObject<Ipv4ClickRouting>(), forwarding);
		unicasts += readUnicasts(n.Get(i)->GetObject<Ipv4ClickRouting>(), forwarding);
	}
	NS_LOG_INFO("STAT PDR        " << ((double) numPidsRecv / numPidsSent) << " (" << numPidsRecv << "/" << numPidsSent << ")");
	NS_LOG_INFO("STAT BU         " << bandwidthUsage << " bytes");
	NS_LOG_INFO("     per PKT    " << ((double) bandwidthUsage / numPktsSent));
	NS_LOG_INFO("     per PID    " << ((double) bandwidthUsage / numPidsSent));
	NS_LOG_INFO("STAT HOP COUNT  " << numPktsForwarded);
	NS_LOG_INFO("     per PKT    " << ((double) numPktsForwarded / numPktsSent));
	NS_LOG_INFO("     per PID    " << ((double) numPktsForwarded / numPidsSent));
	NS_LOG_INFO("STAT BROADCAST  " << ((double) broadcasts / (unicasts + broadcasts)) << " (" << broadcasts << "/" << (unicasts + broadcasts) << ")");

	//
	// Cleanup
	//
	Simulator::Destroy();

#else
	NS_FATAL_ERROR ("Can't use ns-3-click without NSCLICK compiled in");
#endif
}
