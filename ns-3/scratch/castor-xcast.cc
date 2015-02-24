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
#include <stdint.h>
#include <numeric>
#include <functional>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-click-routing.h"
#include "ns3/click-internet-stack-helper.h"
#include "ns3/random-variable-stream.h"
#include "ns3/config.h"
#include "ns3/netanim-module.h"

#ifndef CLICK_PATH
#define CLICK_PATH "/home/milan/click"
#endif

#define INVALID_NUMBER SIZE_MAX

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NsclickCastor");

#ifdef NS3_CLICK

/**
 * Depending on which click configuration we use, we might have different locations of the
 * Recorder elements.
 */
class ClickRecorderLookup {
public:
	static std::string pktForward() { return handlepktPrefix() + "forward/rec"; }
	static std::string pktSend()    { return "handleIpPacket/rec"; }
	static std::string pktDeliver() { return handlepktPrefix() + "handleLocal/rec"; }
	static std::string ackForward() { return handleackPrefix() + "sendAck/recAck"; }
	static std::string ackSend()    { return handlepktPrefix() + "sendAck/recAck"; }
	static std::string pktDrop()    { return handlepktPrefix() + "blackhole/rec"; }
	static void setXcastPromisc(bool b) { isXcastPromisc = b; }
	static void setUnicastPromisc(bool b) { isUnicastPromisc = b; }
private:
	static bool isXcastPromisc;
	static bool isUnicastPromisc;
	static std::string handlepktPrefix() {
		std::string handlepktPrefix = "handlepkt/";
		if (isXcastPromisc) handlepktPrefix.append("handleXcastPkt/");
		return handlepktPrefix;
	}
	static std::string handleackPrefix() {
		std::string handleackPrefix = "handleack/";
		if (isXcastPromisc) handleackPrefix.append("handleXcastAck/");
		else if (isUnicastPromisc) handleackPrefix.append("handleCastorAck/");
		return handleackPrefix;
	}
};
bool ClickRecorderLookup::isXcastPromisc = false;
bool ClickRecorderLookup::isUnicastPromisc = false;


std::string readStringStat(Ptr<Ipv4ClickRouting> clickRouter, std::string what, std::string where) {
	return clickRouter->ReadHandler(where, what);
}

size_t readSizeStat(Ptr<Ipv4ClickRouting> clickRouter, std::string where, std::string what) {
	std::string result = readStringStat(clickRouter, where, what);
	size_t i = INVALID_NUMBER;
	sscanf(result.c_str(), "%zu", &i);
	return i;
}

class Metric {
public:
	Metric(std::string name) : name(name) {};
	virtual ~Metric() {};
	virtual void read() = 0;

	void write(std::string basefilename) {
		std::ofstream outfile;
		for (auto writer : writers) {
			outfile.open((basefilename + "-" + name + "-" + writer.first).c_str());
			if(outfile.fail()) {
				NS_LOG_ERROR("Could not write to file '" << outfile << "'");
				return;
			}
			writer.second(outfile);
			outfile.close();
		}
	}
protected:
	void addWriter(std::string suffix, std::function<void (std::ofstream& out)> writer) {
		writers.push_back(std::make_pair(suffix, writer));
	}
private:
	std::list<std::pair<std::string, std::function<void (std::ofstream& out)>> > writers;
	const std::string name;
};

template<typename Val>
class IntervalMetric : public Metric {
public:
	IntervalMetric(std::string name) : Metric(name) {
		this->addWriter("interval", [&] (std::ofstream& out) {
			auto intervalList = this->intervals();
			for(auto val : intervalList)
				out << val << "\n";
		});
	}
	virtual const std::list<Val> intervals() const {
		return intervalValues;
	}
protected:
	mutable std::list<Val> intervalValues;
};

template<typename Val>
class SimpleIntervalMetric : public IntervalMetric<Val> {
public:
	virtual ~SimpleIntervalMetric() {}
	SimpleIntervalMetric(std::string name) : IntervalMetric<Val>(name) {
		this->addWriter("total", [&] (std::ofstream& out) {
			out << this->total() << "\n";
		});
	}
	virtual Val total() const {
		return std::accumulate(this->intervalValues.begin(), this->intervalValues.end(), 0);
	}
};

template<typename Val>
class DiscreteIntervalMetric : public IntervalMetric<double> {
public:
	virtual ~DiscreteIntervalMetric() {}
	DiscreteIntervalMetric(std::string name) : IntervalMetric<double>(name) {
		this->addWriter("dist", [&] (std::ofstream& out) {
			for (auto val : distribution())
				out << val << "\n";
		});
		this->addWriter("avg", [&] (std::ofstream& out) {
			out << average() << "\n";
		});
	}
	virtual double average() const {
		return std::accumulate(values.begin(), values.end(), 0.0, [&] (const double& a, const Val& b) {
			return a + (double) b / values.size(); // Divide before adding to avoid overflow
		});
	}
	virtual const std::list<Val> distribution() const {
		return values;
	}
protected:
	std::list<Val> values;
};

class NeighborCount : public DiscreteIntervalMetric<uint16_t> {
public:
	NeighborCount(std::string name, const NodeContainer& nodes) : DiscreteIntervalMetric<uint16_t>(name), nodes(nodes) {};
	void read() {
		double avg = 0;
		for (auto n = nodes.Begin(); n != nodes.End(); n++) {
			auto count = readSizeStat((*n)->GetObject<Ipv4ClickRouting>(), "num", "neighbors");
			values.push_back(count);
			avg += (double) count / nodes.GetN();
		}
		intervalValues.push_back(avg);
	}
private:
	const NodeContainer& nodes;
};

class HopCount : public DiscreteIntervalMetric<uint16_t> {
public:
	HopCount(std::string name, const NodeContainer& nodes) : DiscreteIntervalMetric<uint16_t>(name), nodes(nodes) {};
	void read() {
		size_t accum = 0;
		size_t size = values.size();
		for (auto n = nodes.Begin(); n != nodes.End(); n++) {
			size_t hc;
			while((hc = readSizeStat((*n)->GetObject<Ipv4ClickRouting>(), "seq_hopcount", ClickRecorderLookup::pktDeliver())) != INVALID_NUMBER) {
				values.push_back(hc);
				accum += hc;
			}
		}
		size_t diff = values.size() - size;
		intervalValues.push_back((double) accum / diff);
	}
private:
	const NodeContainer& nodes;
};

/**
 * Delay in seconds
 */
class Delay : public DiscreteIntervalMetric<double> {
public:
	Delay(std::string name, const NodeContainer& nodes) : DiscreteIntervalMetric<double>(name), nodes(nodes) {};
	void read() {
		std::list<std::pair<std::string, double> > pidsReceived; // We only need this temporarily
		for (auto n = nodes.Begin(); n != nodes.End(); n++) {
			std::string pid;
			double timestamp;
			while (readPidTimestamp((*n)->GetObject<Ipv4ClickRouting>(), ClickRecorderLookup::pktSend(), pid, timestamp))
				pidsSent.insert(std::make_pair(pid, timestamp));
			while (readPidTimestamp((*n)->GetObject<Ipv4ClickRouting>(), ClickRecorderLookup::pktDeliver(), pid, timestamp))
				pidsReceived.push_back(std::make_pair(pid, timestamp));
		}
		double avg = 0;
		for (auto entry : pidsReceived) {
			std::string pid = entry.first;
			NS_ASSERT_MSG(pidsSent.count(pid) == 1, "" << entry.first << " was received but never sent");
			double sentTimestamp = pidsSent[pid];
			double receivedTimestamp = entry.second;
			NS_ASSERT_MSG(sentTimestamp < receivedTimestamp, "" << entry.first << " was received before it was sent");
			double delay = receivedTimestamp - sentTimestamp;
			values.push_back(delay);
			avg += delay / pidsReceived.size();
		}
		intervalValues.push_back(avg);
	}
private:
	const NodeContainer& nodes;
	std::map<std::string, double> pidsSent;
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
};

class PhyTx : public SimpleIntervalMetric<size_t> {
public:
	PhyTx(std::string name) : SimpleIntervalMetric<size_t>(name) {
		current = 0;
		Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxBegin", MakeCallback(&PhyTx::addPacket, this));
	}
	void read() {
		intervalValues.push_back(current);
		current = 0;
	}
	void addPacket(Ptr<const Packet> p) {
		current += p->GetSize();
	}
private:
	size_t current;
};

class PhyRxDrops : public SimpleIntervalMetric<size_t> {
public:
	PhyRxDrops(std::string name) : SimpleIntervalMetric<size_t>(name) {
		current = 0;
		Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop", MakeCallback(&PhyRxDrops::addPacket, this));
	}
	void read() {
		intervalValues.push_back(current);
		current = 0;
	}
	void addPacket(Ptr<const Packet> p) {
		current ++;
	}
private:
	size_t current;
};

class AccumIntervalMetric : public SimpleIntervalMetric<size_t> {
public:
	AccumIntervalMetric(std::string name, const NodeContainer& nodes, std::string where, std::string what) :
		SimpleIntervalMetric<size_t>(name), nodes(nodes) {
		this->where.push_back(where);
		this->what.push_back(what);
	}
	void read() {
		size_t accum = 0;
		for (auto n = nodes.Begin(); n != nodes.End(); n++)
			for (auto where : this->where)
				for (auto what : this->what)
					accum += readSizeStat((*n)->GetObject<Ipv4ClickRouting>(), what, where);
		intervalValues.push_back(accum);
	}
protected:
	inline void add(std::string where) {
		this->where.push_back(where);
	}
private:
	const NodeContainer& nodes;
	std::list<std::string> where;
	std::list<std::string> what;
};

template<typename Val, typename T1, typename T2>
class CombinedAccumIntervalMetric : public SimpleIntervalMetric<Val> {
public:
	CombinedAccumIntervalMetric(std::string name, const SimpleIntervalMetric<T1>* m1, const SimpleIntervalMetric<T2>* m2, std::function<Val (T1, T2)> op) :
		SimpleIntervalMetric<Val>(name), m1(m1), m2(m2), op(op) {};
	void read() { /* does nothing */ }
	const std::list<Val> intervals() const {
		auto list1 = m1->intervals();
		auto list2 = m2->intervals();
		NS_ASSERT(m1->intervals().size() == m2->intervals().size());
		this->intervalValues.clear();
		auto it1 = list1.begin();
		auto it2 = list2.begin();
		for (; it1 != list1.end() && it2 != list2.end(); ++it1, ++it2) {
			this->intervalValues.push_back(op(*it1, *it2));
		}
		return this->intervalValues;
	}
	Val total() const {
		return op(m1->total(), m2->total());
	}
private:
	const SimpleIntervalMetric<T1>* m1;
	const SimpleIntervalMetric<T2>* m2;
	std::function<Val (T1, T2)> op;
};

template<typename T1, typename T2>
class NormalizedAccumIntervalMetric : public CombinedAccumIntervalMetric<double, T1, T2> {
public:
	NormalizedAccumIntervalMetric(std::string name, const SimpleIntervalMetric<T1>* metric, const SimpleIntervalMetric<T2>* ref) :
		CombinedAccumIntervalMetric<double, T1, T2>(name, metric, ref, [] (T1 v1, T2 v2) { return (double) v1 / (double) v2; }) {};
};

template<typename T>
class AdditiveAccumIntervalMetric : public CombinedAccumIntervalMetric<T, T, T> {
public:
	AdditiveAccumIntervalMetric(std::string name, const SimpleIntervalMetric<T>* metric, const SimpleIntervalMetric<T>* ref) :
		CombinedAccumIntervalMetric<T, T, T>(name, metric, ref, [] (T v1, T v2) { return v1 + v2; }) {};
};

class BlackholeDrops : public AccumIntervalMetric {
public:
	BlackholeDrops(std::string name, const NodeContainer& nodes) :
		AccumIntervalMetric(name, nodes, ClickRecorderLookup::pktDrop(), "npids") {}
};

class UnicastCount : public AccumIntervalMetric {
public:
	UnicastCount(std::string name, const NodeContainer& nodes) :
		AccumIntervalMetric(name, nodes, ClickRecorderLookup::pktForward(), "nunicasts") {}
};

class BroadcastCount : public AccumIntervalMetric {
public:
	BroadcastCount(std::string name, const NodeContainer& nodes) :
		AccumIntervalMetric(name, nodes, ClickRecorderLookup::pktForward(), "nbroadcasts") {}
};

class PidCount : public AccumIntervalMetric {
public:
	PidCount(std::string name, const NodeContainer& nodes, std::string where) :
		AccumIntervalMetric(name, nodes, where, "npids") {}
};

class PacketCount : public AccumIntervalMetric {
public:
	PacketCount(std::string name, const NodeContainer& nodes, std::string where) :
		AccumIntervalMetric(name, nodes, where, "npackets") {}
};

class BandwidthUsage : public AccumIntervalMetric {
public:
	BandwidthUsage(std::string name, const NodeContainer& nodes, std::string where, std::string what) :
		AccumIntervalMetric(name, nodes, where, what) {}
	BandwidthUsage(std::string name, const NodeContainer& nodes, std::list<std::string> where, std::string what) :
		AccumIntervalMetric(name, nodes, where.front(), what) {
		NS_ASSERT(where.size() > 0);
		auto it = where.begin();
		for (it++; it != where.end(); it++)
			add(*it);
	}
};

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

	for (size_t i = 0; i < destinations.size(); i++) {
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
	for (uint32_t i = 0; i < nodes.GetN(); i++)
		clickinternet.SetClickFile(nodes.Get(i), clickConfig.Get());
	clickinternet.SetRoutingTableElement(nodes, "rt");
	clickinternet.Install(nodes);
}

void setBlackHoles(NodeContainer& nodes, size_t nBlackholes) {
	for (uint32_t i = nodes.GetN() - 1; i >= nodes.GetN() - nBlackholes; i--)
		Simulator::Schedule(Seconds(0.5), &WriteSetBlackhole, nodes.Get(i)->GetObject<Ipv4ClickRouting>(), true);
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

std::string printTime(const time_t* time) {
	struct tm* t = localtime(time);
	char buf[20];
	strftime(buf, 20, "%F %T", t); // YYYY-MM-DD HH:MM:SS
	return std::string(buf);
}

void simulate(
		int run,
		StringValue clickConfig,
		Time duration,
		const NetworkConfiguration& netConfig,
		const TrafficConfiguration& trafficConfig,
		const MobilityConfiguration& mobilityConfig,
		double blackholeFraction,
		std::string outFile,
		bool enableAnim
		) {

	RngSeedManager::SetSeed(12345);
	RngSeedManager::SetRun(run);

	Time gratiousDelta = Seconds(1.0);
	Time startSimulation = Seconds(0.0);
	Time startTraffic = startSimulation + gratiousDelta;
	Time endTraffic = startTraffic + duration;
	Time endSimulation = endTraffic + gratiousDelta;

	bool isFlooding = clickConfig.Get() == CLICK_PATH"/conf/castor/flooding.click";  // TODO quick'n'dirty
	ClickRecorderLookup::setXcastPromisc(clickConfig.Get() == CLICK_PATH"/conf/castor/castor_xcast_routing_promisc.click");
	ClickRecorderLookup::setUnicastPromisc(clickConfig.Get() == CLICK_PATH"/conf/castor/castor_multicast_via_unicast_routing.click" ||
										   clickConfig.Get() == CLICK_PATH"/conf/castor/castor_multicast_via_unicast_routing_v2.click");

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
	std::map<Ipv4Address, std::vector<Ptr<Node> > > groups; // multicast group -> xcast receivers

	for (size_t i = 0, iAddr = 1; i < nSenders; i++) {
		//Ipv4Address sender = Ipv4Address(baseAddr.Get() + iAddr);
		Ptr<Node> sender = n.Get(iAddr - 1);
		Ipv4Address group = Ipv4Address(groupAddr.Get() + iAddr);
		std::vector<Ptr<Node> > xcastDestinations;
		iAddr = iAddr % netConfig.nNodes + 1; // Circular count from 1..nNodes
		for (size_t j = 0; j < trafficConfig.groupSize; j++, iAddr = iAddr % netConfig.nNodes + 1)
			xcastDestinations.push_back(n.Get(iAddr - 1));
		senderGroupAssign.insert(std::make_pair(sender, group));
		groups.insert(std::make_pair(group, xcastDestinations));
	}

	// Set up traffic generation
	ApplicationContainer apps;
	uint16_t port = 4242;
	size_t nodeIndex = 0;
	for (std::map<Ptr<Node>, Ipv4Address>::iterator it = senderGroupAssign.begin(); it != senderGroupAssign.end(); it++, nodeIndex++) {
		Ptr<Node> sender = it->first;
		Ipv4Address groupIp = it->second;
		// Setup multicast source
		UdpClientHelper client(groupIp, port);
		client.SetAttribute("MaxPackets", UintegerValue(UINT32_MAX));
		client.SetAttribute("Interval", TimeValue(trafficConfig.sendInterval));
		client.SetAttribute("PacketSize", UintegerValue(packetSize));
		apps = client.Install(NodeContainer(sender));
		apps.Start(startTraffic + trafficConfig.sendInterval / nSenders * nodeIndex);
		apps.Stop(endTraffic);

		std::vector<Ptr<Node> > dsts = groups.at(groupIp);
		for(std::vector<Ptr<Node> >::iterator itDst = dsts.begin(); itDst != dsts.end(); itDst++) {
			UdpServerHelper server(port);
			apps = server.Install(*itDst);
			apps.Start(startTraffic);
			apps.Stop(endSimulation);
		}
	}

	// We fill in the ARP tables at the beginning of the simulation
	Simulator::Schedule(startSimulation, &WriteArp, n);
	std::string mapLocation = isFlooding ? "map" : "handleIpPacket/map";
	for (uint32_t i = 0; i < n.GetN(); i++) {
		// Write Xcast destination mapping
		for (std::map<Ipv4Address, std::vector<Ptr<Node> > >::iterator it = groups.begin(); it != groups.end(); it++) {
			Simulator::Schedule(startSimulation, &WriteXcastMap, n.Get(i)->GetObject<Ipv4ClickRouting>(), it->first, it->second, mapLocation);
		}
	}

	//
	// Schedule evaluation
	//
	const Time interval = Seconds(1.0);
	PhyTx buPhy("phytx");
	PhyRxDrops phyrxdrops("phydrops");
	BlackholeDrops blackholedrops("bh_drops", n);
	NeighborCount neighbors("neighbors", n);
	HopCount hopcount("hopcount", n);
	Delay delay("delay", n);
	UnicastCount unicasts("", n);
	BroadcastCount broadcasts("", n);
	PidCount pidSent("", n, ClickRecorderLookup::pktSend());
	BandwidthUsage buBroadcastPkt("", n, ClickRecorderLookup::pktForward(), "size_broadcast");
	BandwidthUsage buBroadcastAck("", n, std::list<std::string>{ ClickRecorderLookup::ackForward(), ClickRecorderLookup::ackSend() }, "size_broadcast");
	BandwidthUsage buUnicastPkt("", n, ClickRecorderLookup::pktForward(), "size_unicast");
	BandwidthUsage buUnicastAck("", n, std::list<std::string>{ ClickRecorderLookup::ackForward(), ClickRecorderLookup::ackSend() }, "size_unicast");
	PacketCount pktDelivered("", n, ClickRecorderLookup::pktDeliver());
	PacketCount pktForward("", n, ClickRecorderLookup::pktForward());
	for (Time t = startTraffic + interval; t <= endTraffic; t += interval) {
		Simulator::Schedule(t, &NeighborCount::read, &neighbors);
		Simulator::Schedule(t, &HopCount::read, &hopcount);
		Simulator::Schedule(t, &Delay::read, &delay);
		Simulator::Schedule(t, &PhyTx::read, &buPhy);
		Simulator::Schedule(t, &PhyRxDrops::read, &phyrxdrops);
		Simulator::Schedule(t, &BlackholeDrops::read, &blackholedrops);
		Simulator::Schedule(t, &UnicastCount::read, &unicasts);
		Simulator::Schedule(t, &BroadcastCount::read, &broadcasts);
		Simulator::Schedule(t, &PidCount::read, &pidSent);
		Simulator::Schedule(t, &PacketCount::read, &pktDelivered);
		Simulator::Schedule(t, &PacketCount::read, &pktForward);
		Simulator::Schedule(t, &BandwidthUsage::read, &buBroadcastPkt);
		Simulator::Schedule(t, &BandwidthUsage::read, &buBroadcastAck);
		Simulator::Schedule(t, &BandwidthUsage::read, &buUnicastPkt);
		Simulator::Schedule(t, &BandwidthUsage::read, &buUnicastAck);
	}

	setBlackHoles(n, round(netConfig.nNodes * blackholeFraction));

	// Create NetAnim traces
	AnimationInterface* anim = 0;
	if (enableAnim && outFile != "") {
		anim = new AnimationInterface(outFile + "-animation.xml");
		anim->SetMobilityPollInterval (Seconds (0.25));
		anim->SetStartTime (startSimulation);
		anim->SetStopTime (endTraffic);
		anim->EnablePacketMetadata(true);
	}

	//
	// Now, do the actual simulation.
	//
	time_t start; time(&start);
	NS_LOG_INFO("START '" << outFile << "' @ " << printTime(&start));
	Simulator::Stop(endSimulation);
	Simulator::Run();
	time_t end; time(&end);

	delete anim;

	NS_LOG_INFO("FINISH '" << outFile << "'");
	NS_LOG_INFO("  @ " << printTime(&end) << " (in " << difftime(end, start) << " seconds)");
	NS_LOG_INFO("  CONFIG " << "Run #" << run << ", simtime " << duration.GetSeconds() << " seconds");
	NS_LOG_INFO("  CONFIG " << "output " << clickConfig.Get());
	NS_LOG_INFO("  CONFIG " << netConfig.x << "x" << netConfig.y << ", " << netConfig.nNodes << " nodes @ " << netConfig.range << " range");
	NS_LOG_INFO("  CONFIG " << nSenders << " senders -> " << trafficConfig.groupSize << " each, " << trafficConfig.packetSize << " bytes / " << trafficConfig.sendInterval.GetSeconds() << " s");
	NS_LOG_INFO("  CONFIG " << "speed " << mobilityConfig.speed << ", pause " << mobilityConfig.pause);
	NS_LOG_INFO("  CONFIG " << "blackholes " << blackholeFraction);

	//
	// Post-process evaluation
	//
	NormalizedAccumIntervalMetric<size_t, size_t> pdr("pdr", &pktDelivered, &pidSent);

	AdditiveAccumIntervalMetric<size_t> buPkt("", &buBroadcastPkt, &buUnicastPkt);
	AdditiveAccumIntervalMetric<size_t> buAck("", &buBroadcastAck, &buUnicastAck);
	AdditiveAccumIntervalMetric<size_t> buTotal("", &buPkt, &buAck);

	NormalizedAccumIntervalMetric<size_t, size_t> buPerPidPhy("bu_phy", &buPhy, &pidSent);
	NormalizedAccumIntervalMetric<size_t, size_t> buPerPidNet("bu", &buTotal, &pidSent);
	NormalizedAccumIntervalMetric<size_t, size_t> buPerPidBroadcastPkt("bu_broadcast_pkt", &buBroadcastPkt, &pidSent);
	NormalizedAccumIntervalMetric<size_t, size_t> buPerPidBroadcastAck("bu_broadcast_ack", &buBroadcastAck, &pidSent);
	NormalizedAccumIntervalMetric<size_t, size_t> buPerPidUnicastPkt("bu_unicast_pkt", &buUnicastPkt, &pidSent);
	NormalizedAccumIntervalMetric<size_t, size_t> buPerPidUnicastAck("bu_unicast_ack", &buUnicastAck, &pidSent);
	NormalizedAccumIntervalMetric<size_t, size_t> buPerPidBroadcastPktFrac("", &buBroadcastPkt, &buTotal);
	NormalizedAccumIntervalMetric<size_t, size_t> buPerPidBroadcastAckFrac("", &buBroadcastAck, &buTotal);
	NormalizedAccumIntervalMetric<size_t, size_t> buPerPidUnicastPktFrac("", &buUnicastPkt, &buTotal);
	NormalizedAccumIntervalMetric<size_t, size_t> buPerPidUnicastAckFrac("", &buUnicastAck, &buTotal);
	NormalizedAccumIntervalMetric<size_t, size_t> buPerPidPkt("", &buPkt, &pidSent);
	NormalizedAccumIntervalMetric<size_t, size_t> buPerPidAck("", &buAck, &pidSent);

	AdditiveAccumIntervalMetric<size_t> decisions("", &unicasts, &broadcasts);
	NormalizedAccumIntervalMetric<size_t, size_t> broadcastFrac("broadcast", &broadcasts, &decisions);

	size_t numGroupMessagesSent = (size_t) ceil (duration.ToDouble(Time::S)  / trafficConfig.sendInterval.ToDouble(Time::S) * nSenders);
	double hopsPerGroupMessage = (double) pktForward.total() / numGroupMessagesSent;

	NS_ASSERT(delay.distribution().size() == pktDelivered.total());

	NS_LOG_INFO("  STAT PDR               " << pdr.total() << " (" << pktDelivered.total() << "/" << pidSent.total() << ")");
	NS_LOG_INFO("  STAT BU PER PID        " << buPerPidPhy.total()  << " (PHY), " << buPerPidNet.total() << " (NET) bytes");
	NS_LOG_INFO("      % BROADCAST PKT    " << buPerPidBroadcastPktFrac.total());
	NS_LOG_INFO("      % BROADCAST ACK    " << buPerPidBroadcastAckFrac.total());
	NS_LOG_INFO("      % UNICAST   PKT    " << buPerPidUnicastPktFrac.total());
	NS_LOG_INFO("      % UNICAST   ACK    " << buPerPidUnicastAckFrac.total());
	NS_LOG_INFO("  STAT DELAY             " << (delay.average() * 1000) << " ms");
	NS_LOG_INFO("  STAT HOP COUNT TO DEST " << hopcount.average());
	NS_LOG_INFO("  STAT GRP MSG HOP COUNT " << hopsPerGroupMessage);
	NS_LOG_INFO("  STAT NEIGHBOR COUNT    " << neighbors.average());
	NS_LOG_INFO("  STAT BROADCAST         " << broadcastFrac.total() << " (" << broadcasts.total() << "/" << decisions.total() << ")");
	NS_LOG_INFO("  STAT PHY RX DROPS      " << phyrxdrops.total());
	NS_LOG_INFO("  STAT ATTACK DROPS      " << blackholedrops.total());

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

	out << pdr.total() << " "
		<< buPerPidPhy.total() << " "
		<< buPerPidNet.total() << " "
		<< buPerPidPkt.total() << " "
		<< buPerPidAck.total() << " "
		<< (delay.average() * 1000) << " "
		<< hopsPerGroupMessage << " "
		<< broadcastFrac.total() << " "
		<< phyrxdrops.total() << " "
		<< blackholedrops.total();

	out.close();

	pdr.write(outFile);
	buPerPidPhy.write(outFile);
	buPerPidNet.write(outFile);
	buPerPidBroadcastPkt.write(outFile);
	buPerPidBroadcastAck.write(outFile);
	buPerPidUnicastPkt.write(outFile);
	buPerPidUnicastAck.write(outFile);
	broadcastFrac.write(outFile);
	neighbors.write(outFile);
	hopcount.write(outFile);
	delay.write(outFile);
}

#endif

int main(int argc, char *argv[]) {
#ifdef NS3_CLICK

	// Possible run configurations

	std::map<std::string, std::string> clickConfigs;
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
	trafficConfigs.insert(std::make_pair( "1_1", TrafficConfiguration(0.01,  1)));
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
	bool enableAnim			   = false;

	cmd.AddValue("run",        "The instance of this experiment.",                       run);
	cmd.AddValue("duration",   "The simulated time in seconds.",                         duration);
	cmd.AddValue("click",      "The Click configuration file to be used.",               click);
	cmd.AddValue("network",    "The network configuration to use.",                      networkConfig);
	cmd.AddValue("traffic",    "The traffic configuration to use.",                      trafficConfig);
	cmd.AddValue("mobility",   "The mobility model and configuration to use.",           mobilityConfig);
	cmd.AddValue("blackholes", "Percentage of nodes the will be acting as a blackhole.", blackholeFraction);
	cmd.AddValue("outfile",    "File for statistics output.",							 outFile);
	cmd.AddValue("anim",	   "Whether to record a NetAnim tracefile.", 				 enableAnim);

	cmd.Parse (argc, argv);

	simulate(run, clickConfigs[click], Seconds(duration), networkConfigs[networkConfig], trafficConfigs[trafficConfig], mobilityConfigs[mobilityConfig], blackholeFraction, outFile, enableAnim);

#else
	NS_FATAL_ERROR ("Can't use ns-3-click without NSCLICK compiled in");
#endif
}
