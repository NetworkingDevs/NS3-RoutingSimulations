/**
 * Learn to use PingHelper class from NS3.
 * 
 * There are a lot of ways you can commu-
 * nicate in NS3. PingHelper class helps 
 * to simulate ICMP based output stat.
 */

 // For General Imports
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"

// For Netanim 
#include "ns3/netanim-module.h"

// Don't forget to include this
#include "ns3/internet-apps-module.h"

using namespace ns3;

int main() 
{
    // This is how you can enable Log Component for PingHelper
    Time::SetResolution(Time::NS);
    LogComponentEnable("Ping", LOG_LEVEL_INFO);

    /**
     * From this point beyound in the code till
     * the PingHelper configuration, will be u-
     * sed to set-up topology. Which doesn't 
     * matter because that can vary. 
     * 
     * So, setting up the basic point-to-point
     * topology.
     */
    NodeContainer allNodes;
    allNodes.Create(2);

    PointToPointHelper p2pChannel;
    p2pChannel.SetChannelAttribute("Delay",StringValue("1ms"));
    p2pChannel.SetDeviceAttribute("DataRate",StringValue("100Mbps"));

    NetDeviceContainer devices;
    devices = p2pChannel.Install(allNodes);

    InternetStackHelper IpStack;
    IpStack.Install(allNodes);

    Ipv4AddressHelper ip;
    ip.SetBase("192.168.10.0",Ipv4Mask("/24"));
    Ipv4InterfaceContainer interfaces = ip.Assign(devices);

    /**
     * This is how you can configure PingHelper.
     */
    uint32_t packetSize = 1024;
    Time interPacketInterval = Seconds(1.0);
    PingHelper ping(Ipv4Address("192.168.10.2"));

    ping.SetAttribute("Interval", TimeValue(interPacketInterval));
    ping.SetAttribute("Size", UintegerValue(packetSize));
    ping.SetAttribute("Count", UintegerValue(5));

    ApplicationContainer apps = ping.Install(allNodes.Get(0));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));


    /**
     * Setting up simulator
     */
    Simulator::Stop(Seconds(20.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}