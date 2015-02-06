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
#include <climits>
#include <stdint.h>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-click-routing.h"
#include "ns3/click-internet-stack-helper.h"
#include "ns3/random-variable-stream.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/config.h"

#ifndef CLICK_PATH
#define CLICK_PATH "/home/milan/click"
#endif

#ifndef UINT32_MAX
#define UINT32_MAX std::numeric_limits<uint32_t>::max()
#endif

using namespace ns3;

#ifndef NS3_CLICK
#define NS3_CLICK
#endif

NS_LOG_COMPONENT_DEFINE("NsclickCastor");

#ifdef NS3_CLICK

uint32_t phyTx = 0;
uint32_t phyRxDrop = 0;

void
PhyTx(Ptr<const Packet> p)
{
	phyTx += p->GetSize();
}

void
PhyRxDrop(Ptr<const Packet> p)
{
	phyRxDrop++;
}

std::string readStringStat(Ptr<Ipv4ClickRouting> clickRouter, std::string what, std::string where) {
	return clickRouter->ReadHandler(where, what);
}

int readIntStat(Ptr<Ipv4ClickRouting> clickRouter, std::string what, std::string where) {
	std::string result = readStringStat(clickRouter, what, where);
	int i;
	sscanf(result.c_str(), "%d", &i);
	return i;
}

double readDoubleStat(Ptr<Ipv4ClickRouting> clickRouter, std::string what, std::string where) {
	std::string result = readStringStat(clickRouter, what, where);
	double i;
	sscanf(result.c_str(), "%lf", &i);
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

int readNextHopcount(Ptr<Ipv4ClickRouting> clickRouter, std::string where) {
	return readIntStat(clickRouter, "seq_hopcount", where);
}

int readNextSizeInterval(Ptr<Ipv4ClickRouting> clickRouter, std::string where) {
	return readIntStat(clickRouter, "size_interval", where);
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

bool readPidTimestamp(Ptr<Ipv4ClickRouting> clickRouter, std::string where, std::string& pid, double& timestamp) {
	std::string result = readStringStat(clickRouter, "seq_entry", where);
	if(result.empty())
		return false;
	std::vector<std::string> entry = split(result, ' '); // result is in form of '<pid> <timestamp>'
	pid = entry[0];
	sscanf(entry[1].c_str(), "%lf", &timestamp);
	return true;
}

void WriteArp(NodeContainer n) {

	for (NodeContainer::Iterator it = n.Begin(); it != n.End(); ++it) {
		Ptr<Node> node = *it;

		Ipv4Address ipv4Addr = node->GetObject<Ipv4ClickRouting>()->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
		Mac48Address macAddr = DynamicCast<WifiNetDevice>(node->GetDevice(0))->GetMac()->GetAddress();

		// Create entry of form "<IP> <MAC>"
		std::stringstream stream;
		stream << ipv4Addr << " " << macAddr;

		for (NodeContainer::Iterator it2 = n.Begin(); it2 != n.End(); ++it2) {
			Ptr<Ipv4ClickRouting> clickRouter = (*it2)->GetObject<Ipv4ClickRouting>();
			clickRouter->WriteHandler("arpquerier", "insert", stream.str().c_str());
		}

	}

}

void WriteXcastMap(Ptr<Ipv4ClickRouting> clickRouter, Ipv4Address group, const std::vector<Ptr<Node> >& destinations, std::string mapLocation) {
	// Create entry of form "<GroupAddr> <Dest1Addr> <Dest2Addr> ... <DestNAddr>"
	std::stringstream stream;

	// Multicast address is first in list
	group.Print(stream);

	for (unsigned int i = 0; i < destinations.size(); i++) {
		stream << " ";
		int externalInterface = 1;
		destinations.at(i)->GetObject<Ipv4>()->GetAddress(externalInterface, 0).GetLocal().Print(stream);
	}

	clickRouter->WriteHandler(mapLocation, "insert", stream.str().c_str());
}

void WriteSetBlackhole(Ptr<Ipv4ClickRouting> clickRouter, bool active) {
	clickRouter->WriteHandler("handlepkt/blackhole/filter", "active", active ? "true" : "false");
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
	//wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel", "Speed", DoubleValue(299792458.0));
	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

	wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(transmissionRange));
	//wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
	wifiPhy.SetChannel(wifiChannel.Create());

	// Add a non-QoS upper mac, and disable rate control
	NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default();
	wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
			StringValue(phyMode), "ControlMode", StringValue(phyMode));
	// Set it to adhoc mode
	wifiMac.SetType("ns3::AdhocWifiMac");

	NetDeviceContainer d = wifi.Install(wifiPhy, wifiMac, nodes);

//	wifiPhy.EnablePcap("castor-xcast", d);

	return d;
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

	mobility.SetMobilityModel("ns3::SteadyStateRandomWaypointMobilityModel",
			"MaxSpeed", DoubleValue(speed),
			"MinX", DoubleValue(0),
			"MaxX", DoubleValue(xSize),
			"MinY", DoubleValue(0),
			"MaxY", DoubleValue(ySize));
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

void setBlackHoles(NodeContainer& nodes, unsigned int nBlackholes) {
	for (unsigned int i = nodes.GetN() - 1; i >= nodes.GetN() - nBlackholes; i--)
		Simulator::Schedule(Seconds(0.5), &WriteSetBlackhole, nodes.Get(i)->GetObject<Ipv4ClickRouting>(), true);
}

template<typename T>
void writeToFile(std::string filename, std::list<T> values) {
	std::ofstream outfile;
	outfile.open(filename.c_str());
	if(outfile.fail()) {
		NS_LOG_ERROR("Could not write to file '" << outfile << "'");
		return;
	}
	for (std::list<unsigned int>::iterator it = values.begin(); it != values.end(); it++)
		outfile << *it << "\n";
	outfile.close();
}

typedef struct NetworkConfiguration {
	double x, y;
	double range;
	size_t nNodes;
	NetworkConfiguration(double x, double y, double range, size_t nNodes) :
		x(x), y(y), range(range), nNodes(nNodes) {}
	NetworkConfiguration() : x(0), y(0), range(0), nNodes(0) {}
} NetworkConfiguration;

typedef struct TrafficConfiguration {
	double senderFraction;
	size_t groupSize;
	size_t packetSize;
	Time sendInterval;
	TrafficConfiguration(double senderFraction, size_t groupSize, size_t packetSize = 256, Time sendInterval = Seconds(0.25)) :
		senderFraction(senderFraction), groupSize(groupSize), packetSize(packetSize), sendInterval(sendInterval) {}
	TrafficConfiguration() : senderFraction(0), groupSize(0), packetSize(256), sendInterval(Seconds(0.25)) {}
} TrafficConfiguration;

typedef struct MobilityConfiguration {
	double speed, pause;
	MobilityConfiguration(double speed, double pause) :
		speed(speed), pause(pause) {}
	MobilityConfiguration() : speed(0), pause(0) {}
} MobilityConfiguration;

void simulate(
		int run,
		StringValue clickConfig,
		Time duration,
		const NetworkConfiguration& netConfig,
		const TrafficConfiguration& trafficConfig,
		const MobilityConfiguration& mobilityConfig,
		double blackholeFraction,
		std::string outFile
		) {

	RngSeedManager::SetSeed(12345);
	RngSeedManager::SetRun(run);

	bool isFlooding = clickConfig.Get() == CLICK_PATH"/conf/castor/flooding.click";  // TODO quick'n'dirty
	bool isXcastPromisc = clickConfig.Get() == CLICK_PATH"/conf/castor/castor_xcast_routing_promisc.click";
	bool isUnicastPromisc = clickConfig.Get() == CLICK_PATH"/conf/castor/castor_multicast_via_unicast_routing.click" ||
							clickConfig.Get() == CLICK_PATH"/conf/castor/castor_multicast_via_unicast_routing_v2.click";

	size_t nSenders = (size_t) ceil(netConfig.nNodes * trafficConfig.senderFraction);

	uint32_t packetSize = trafficConfig.packetSize - 28; // IP+UDP header size = 28 byte
	if (isFlooding)
		packetSize += 14; // Currently, we wrap Castor PKTs into IP packets before sending, so add sizeof IP header for better comparability

	// Network
	const Ipv4Address baseAddr("10.0.0.0");
	const Ipv4Mask networkMask("255.0.0.0");
	const Ipv4Address groupAddr("224.0.2.0");

	// Set up network
	NodeContainer n;
	n.Create(netConfig.nNodes);

	NetDeviceContainer d = setPhysicalChannel(n, netConfig.range);
	if(mobilityConfig.speed == 0.0)
		setConstantPositionMobility(n, netConfig.x, netConfig.y);
	else
		setRandomWaypointMobility(n, netConfig.x, netConfig.y, mobilityConfig.speed, mobilityConfig.pause);

	setClickRouter(n, clickConfig);

	Ipv4AddressHelper ipv4;
	ipv4.SetBase(baseAddr, networkMask);
	Ipv4InterfaceContainer i = ipv4.Assign(d);

	// Setup groups based on 'nSenders' (e.g. 2) and 'groupSize' (e.g. 3) in the format:
	// 'senderGroupAssign':
	// 		192.168.201.1 -> 224.0.2.1; 192.168.201.5 -> 224.0.2.5
	std::map<Ptr<Node>, Ipv4Address> senderGroupAssign;
	// 'groups':
	// 		192.168.201.1 -> 192.168.201.2, 192.168.201.3, 192.168.201.4;
	// 		192.168.201.5 -> 192.168.201.6, 192.168.201.7, 192.168.201.8
	std::map<Ipv4Address, std::vector<Ptr<Node> >, LessIpv4Address> groups; // multicast group -> xcast receivers

	for (unsigned int i = 0, iAddr = 1; i < nSenders; i++) {
		//Ipv4Address sender = Ipv4Address(baseAddr.Get() + iAddr);
		Ptr<Node> sender = n.Get(iAddr - 1);
		Ipv4Address group = Ipv4Address(groupAddr.Get() + iAddr);
		std::vector<Ptr<Node> > xcastDestinations;
		iAddr = iAddr % netConfig.nNodes + 1; // Circular count from 1..nNodes
		for (unsigned int j = 0; j < trafficConfig.groupSize; j++, iAddr = iAddr % netConfig.nNodes + 1)
			xcastDestinations.push_back(n.Get(iAddr - 1));
		senderGroupAssign.insert(std::make_pair(sender, group));
		groups.insert(std::make_pair(group, xcastDestinations));
	}

	// Set up traffic generation
	ApplicationContainer apps;
	uint16_t port = 4242;
	unsigned int nodeIndex = 0;
	for (std::map<Ptr<Node>, Ipv4Address>::iterator it = senderGroupAssign.begin(); it != senderGroupAssign.end(); it++, nodeIndex++) {
		Ptr<Node> sender = it->first;
		Ipv4Address groupIp = it->second;
		// Setup multicast source
		UdpClientHelper client(groupIp, port);
		client.SetAttribute("MaxPackets", UintegerValue(UINT32_MAX));
		client.SetAttribute("Interval", TimeValue(trafficConfig.sendInterval));
		client.SetAttribute("PacketSize", UintegerValue(packetSize));
		apps = client.Install(NodeContainer(sender));
		apps.Start(Seconds(2.0) + trafficConfig.sendInterval / nSenders * nodeIndex);
		apps.Stop(duration + Seconds(2.0));

		std::vector<Ptr<Node> > dsts = groups.at(groupIp);
		for(std::vector<Ptr<Node> >::iterator itDst = dsts.begin(); itDst != dsts.end(); itDst++) {
			UdpServerHelper server(port);
			apps = server.Install(*itDst);
			apps.Start(Seconds(1.0));
			apps.Stop(duration + Seconds(3.0));
		}
	}

	// We fill in the ARP tables at the beginning of the simulation
	Simulator::Schedule(Seconds(0.5), &WriteArp, n);
	std::string mapLocation = isFlooding ? "map" : "handleIpPacket/map";
	for (unsigned int i = 0; i < n.GetN(); i++) {
		// Write Xcast destination mapping
		for (std::map<Ipv4Address, std::vector<Ptr<Node> > >::iterator it = groups.begin(); it != groups.end(); it++) {
			Simulator::Schedule(Seconds(0.5), &WriteXcastMap, n.Get(i)->GetObject<Ipv4ClickRouting>(), it->first, it->second, mapLocation);
		}
	}

	setBlackHoles(n, round(netConfig.nNodes * blackholeFraction));

	// Install FlowMonitor on all nodes
	FlowMonitorHelper flowmon;
	Ptr<FlowMonitor> monitor = flowmon.InstallAll();

	Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxBegin", MakeCallback(&PhyTx));
	Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop", MakeCallback(&PhyRxDrop));

	//
	// Now, do the actual simulation.
	//
	time_t start; time(&start);
	Simulator::Stop(duration + Seconds(6.0));
	Simulator::Run();
	time_t end; time(&end);

	NS_LOG_INFO("Run #" << run << " (" << duration.GetSeconds() << " seconds, " << clickConfig.Get() << ")");
	NS_LOG_INFO("  CONFIG " << netConfig.x << "x" << netConfig.y << ", " << netConfig.nNodes << " nodes @ " << netConfig.range << " range");
	NS_LOG_INFO("  CONFIG " << nSenders << " senders -> " << trafficConfig.groupSize << " each, " << trafficConfig.packetSize << " bytes / " << trafficConfig.sendInterval.GetSeconds() << " s");
	NS_LOG_INFO("  CONFIG " << "speed " << mobilityConfig.speed << ", pause " << mobilityConfig.pause);
	NS_LOG_INFO("  CONFIG " << "blackholes " << blackholeFraction);
	NS_LOG_INFO("  Done after " << difftime(end, start) << " seconds");

	//
	// Schedule evaluation
	//
	uint32_t numPidsSent = 0;
	uint32_t numPktsRecv = 0;
	uint32_t pktBandwidthUsage = 0;
	uint32_t ackBandwidthUsage = 0;
	uint32_t totalBandwidthUsage = 0;
	uint32_t numPktsSent = 0;
	uint32_t numGroupMessagesSent = (uint32_t) (duration.ToDouble(Time::S) * nSenders / trafficConfig.sendInterval.ToDouble(Time::S));
	uint32_t broadcasts = 0;
	uint32_t unicasts = 0;
	uint32_t numPktsForwarded = 0;
	uint32_t numAcksForwarded = 0;
	std::map<std::string, double> pidsSent;
	std::vector<double> delays;
	double avgDelay = 0;
	uint32_t pktDroppedByBlackhole = 0;
	double avgHopcount = 0;
	std::list<uint32_t> hopcounts;
	std::list<uint32_t> buDistribution;

	std::string handlepktPrefix = "handlepkt/";
	if (isXcastPromisc) handlepktPrefix.append("handleXcastPkt/");
	std::string handleackPrefix = "handleack/";
	if (isXcastPromisc) handleackPrefix.append("handleXcastAck/");
	else if (isUnicastPromisc) handleackPrefix.append("handleCastorAck/");
	std::string pktForward = handlepktPrefix + "forward/rec";
	std::string pktSend    = "handleIpPacket/rec";
	std::string pktDeliver = handlepktPrefix + "handleLocal/rec";
	std::string ackForward = handleackPrefix + "sendAck/recAck";
	std::string ackSend    = handlepktPrefix + "sendAck/recAck";
	std::string pktDrop    = handlepktPrefix + "blackhole/rec";
	for(unsigned int i = 0; i < netConfig.nNodes; i++) {
		Ptr<Ipv4ClickRouting> router = n.Get(i)->GetObject<Ipv4ClickRouting>();
		numPidsSent += readPidCount(router, pktSend);
		numPktsRecv += readPktCount(router, pktDeliver);
		pktBandwidthUsage += readAccumPktSize(router, pktForward);
		if(!isFlooding)
			ackBandwidthUsage += readAccumPktSize(router, ackSend) +
							 	 readAccumPktSize(router, ackForward);
		numPktsSent += readPktCount(router, pktSend);
		numPktsForwarded += readPktCount(router, pktForward);
		if(!isFlooding)
			numAcksForwarded += readPktCount(router, ackSend) +
								readPktCount(router, ackForward);
		broadcasts += readBroadcasts(router, pktForward);
		unicasts += readUnicasts(router, pktForward);
		pktDroppedByBlackhole += readPidCount(router, pktDrop);
		for (int hc = readNextHopcount(router, pktDeliver); hc != -1; hc = readNextHopcount(router, pktDeliver)) {
			hopcounts.push_back(hc);
		}
		std::string pid;
		double timestamp;
		while(readPidTimestamp(router, pktSend, pid, timestamp))
			pidsSent.insert(std::make_pair(pid, timestamp));
		if (!isFlooding) { // Flooding currently does not support this
			// Bandwidth utilization over time
			int pktSize;
			std::list<unsigned int>::iterator it = buDistribution.begin();
			while ((pktSize = readNextSizeInterval(router, pktForward)) != -1) {
				if (it == buDistribution.end()) {
					buDistribution.push_back(pktSize);
				} else {
					*it += pktSize;
					it++;
				}
			}
		}
	}
	totalBandwidthUsage = pktBandwidthUsage + ackBandwidthUsage;
	for(unsigned int i = 0; i < netConfig.nNodes; i++) {
		std::string pid;
		double recTimestamp;
		while(readPidTimestamp(n.Get(i)->GetObject<Ipv4ClickRouting>(), pktDeliver, pid, recTimestamp)) {
			NS_ASSERT_MSG(pidsSent[pid] != 0.0, "" << pid << " was received but never sent");
			double sentTimestamp = pidsSent[pid];
			double delay = recTimestamp - sentTimestamp;
			delays.push_back(recTimestamp - sentTimestamp);
			avgDelay += delay;
		}
	}
	for (std::list<uint32_t>::iterator it = hopcounts.begin(); it != hopcounts.end(); it++)
		avgHopcount += (double) *it / (double) hopcounts.size();

	double pdr = (double) numPktsRecv / numPidsSent;
	double delay = avgDelay / numPktsRecv * 1000;
	double buPerPidNet = (double) totalBandwidthUsage / numPidsSent;
	double buPerPidPhy = (double) phyTx / numPidsSent;
	double buPerPidPkt = (double) pktBandwidthUsage / numPidsSent;
	double buPerPidAck = (double) ackBandwidthUsage / numPidsSent;
	double hopsPerGroupMessage = (double) numPktsForwarded / numGroupMessagesSent;

	NS_LOG_INFO("  STAT PDR               " << pdr << " (" << numPktsRecv << "/" << numPidsSent << ")");
	NS_LOG_INFO("  STAT BU per PID        " << buPerPidPhy  << " (phy), " << buPerPidNet << " (net) bytes");
	NS_LOG_INFO("        frac(PKT)        " << ((double) buPerPidPkt / buPerPidNet));
	NS_LOG_INFO("        frac(ACK)        " << ((double) buPerPidAck / buPerPidNet));
	NS_LOG_INFO("  STAT DELAY             " << delay << " ms");
	NS_LOG_INFO("  STAT HOP COUNT TO DEST " << avgHopcount);
	NS_LOG_INFO("  STAT GRP MSG HOP COUNT " << hopsPerGroupMessage);
	NS_LOG_INFO("  STAT BROADCAST         " << ((double) broadcasts / (unicasts + broadcasts)) << " (" << broadcasts << "/" << (unicasts + broadcasts) << ")");
	NS_LOG_INFO("  STAT PHY RX DROPS      " << phyRxDrop);
	NS_LOG_INFO("  STAT ATTACK DROPS      " << pktDroppedByBlackhole);

	//
	// Cleanup
	//
	Simulator::Destroy();

	// Write statistics to output file
	std::ofstream out;
	out.open(outFile.c_str());
	if(out.fail()) {
		NS_LOG_ERROR("Could not write to file '" << outFile << "'");
		return;
	}

	out << pdr << " "
		<< buPerPidPhy << " "
		<< buPerPidNet << " "
		<< ((double) buPerPidPkt * (buPerPidPhy/buPerPidNet)) << " "
		<< ((double) buPerPidAck * (buPerPidPhy/buPerPidNet)) << " "
		<< delay << " "
		<< hopsPerGroupMessage << " "
		<< ((double) broadcasts / (unicasts + broadcasts)) << " "
		<< phyRxDrop << " "
		<< pktDroppedByBlackhole;

	out.close();

	writeToFile(outFile + "-hopcount", hopcounts);
	writeToFile(outFile + "-bu_distribution", buDistribution);

}

#endif

int main(int argc, char *argv[]) {
#ifdef NS3_CLICK

	// Possible run configurations

	std::map<std::string, StringValue> clickConfigs;
	clickConfigs.insert(std::make_pair("xcast",         CLICK_PATH"/conf/castor/castor_xcast_routing.click"));
	clickConfigs.insert(std::make_pair("xcast-promisc", CLICK_PATH"/conf/castor/castor_xcast_routing_promisc.click"));
	clickConfigs.insert(std::make_pair("regular",       CLICK_PATH"/conf/castor/castor_multicast_via_unicast_routing.click"));
	clickConfigs.insert(std::make_pair("regular2",		CLICK_PATH"/conf/castor/castor_multicast_via_unicast_routing_v2.click"));
	clickConfigs.insert(std::make_pair("flooding",      CLICK_PATH"/conf/castor/flooding.click"));


	std::map<std::string, NetworkConfiguration> networkConfigs;
	// Configurations with the same node density
	networkConfigs.insert(std::make_pair("tiny",   NetworkConfiguration( 948.7,  948.7, 500.0,  10)));
	networkConfigs.insert(std::make_pair("small",  NetworkConfiguration(2121.3, 2121.3, 500.0,  50)));
	networkConfigs.insert(std::make_pair("medium", NetworkConfiguration(3000.0, 3000.0, 500.0, 100))); // as in Castor
	networkConfigs.insert(std::make_pair("large",  NetworkConfiguration(4242.6, 4242.6, 500.0, 200)));


	std::map<std::string, TrafficConfiguration> trafficConfigs;
	// as in Castor (5 unicast flows @ 100 nodes)
	trafficConfigs.insert(std::make_pair( "5_1", TrafficConfiguration(0.05,  1)));
	// 40% receivers
	trafficConfigs.insert(std::make_pair("4_10", TrafficConfiguration(0.04, 10)));
	trafficConfigs.insert(std::make_pair( "8_5", TrafficConfiguration(0.08,  5)));
	trafficConfigs.insert(std::make_pair("20_2", TrafficConfiguration(0.20,  2)));
	trafficConfigs.insert(std::make_pair("40_1", TrafficConfiguration(0.40,  1)));
	// 20% receivers
	trafficConfigs.insert(std::make_pair("2_10", TrafficConfiguration(0.02, 10)));
	trafficConfigs.insert(std::make_pair( "4_5", TrafficConfiguration(0.04,  5)));
	trafficConfigs.insert(std::make_pair("10_2", TrafficConfiguration(0.10,  2)));
	trafficConfigs.insert(std::make_pair("20_1", TrafficConfiguration(0.20,  1)));
	// 10% receivers
	trafficConfigs.insert(std::make_pair("1_10", TrafficConfiguration(0.01, 10)));
	trafficConfigs.insert(std::make_pair( "2_5", TrafficConfiguration(0.02,  5)));
	trafficConfigs.insert(std::make_pair( "5_2", TrafficConfiguration(0.05,  2)));
	trafficConfigs.insert(std::make_pair("10_1", TrafficConfiguration(0.10,  1)));

	std::map<std::string, MobilityConfiguration> mobilityConfigs;
	mobilityConfigs.insert(std::make_pair( "0", MobilityConfiguration( 0.0, 0.0)));
	mobilityConfigs.insert(std::make_pair("10", MobilityConfiguration(10.0, 0.0)));
	mobilityConfigs.insert(std::make_pair("20", MobilityConfiguration(20.0, 0.0))); // as in Castor


	// Enable logging
	LogComponentEnable("NsclickCastor", LOG_LEVEL_INFO);

	// Set up command line usage
	CommandLine cmd;
	// Default values
	uint32_t run               = 1;
	double duration            = 60.0;
	std::string click          = "xcast-promisc";
	std::string networkConfig  = "medium";
	std::string trafficConfig  = "4_5";
	std::string mobilityConfig = "20";
	double blackholeFraction   = 0.0;
	std::string outFile		   = "";

	cmd.AddValue("run",        "The instance of this experiment.",                       run);
	cmd.AddValue("duration",   "The simulated time in seconds.",                         duration);
	cmd.AddValue("click",      "The Click configuration file to be used.",               click);
	cmd.AddValue("network",    "The network configuration to use.",                      networkConfig);
	cmd.AddValue("traffic",    "The traffic configuration to use.",                      trafficConfig);
	cmd.AddValue("mobility",   "The mobility model and configuration to use.",           mobilityConfig);
	cmd.AddValue("blackholes", "Percentage of nodes the will be acting as a blackhole.", blackholeFraction);
	cmd.AddValue("outfile",    "File for statistics output.",							 outFile);

	cmd.Parse (argc, argv);

	simulate(run, clickConfigs[click], Seconds(duration), networkConfigs[networkConfig], trafficConfigs[trafficConfig], mobilityConfigs[mobilityConfig], blackholeFraction, outFile);

#else
	NS_FATAL_ERROR ("Can't use ns-3-click without NSCLICK compiled in");
#endif
}
