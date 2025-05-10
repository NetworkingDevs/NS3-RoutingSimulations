/**
 * This script will help you learn the basics of RIP
 * from NS3. This script uses PingHelper and RipHelper
 * to simulate very basic RIP Simulation.
 */
// For General Imports
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
// This is for ping and many others
#include "ns3/internet-apps-module.h"
// This is for NetAnim...
#include "ns3/netanim-module.h"

using namespace ns3;

int main() {
    // Enable logging for server and client...
    Time::SetResolution(Time::NS);
    LogComponentEnable("Ping", LOG_LEVEL_INFO);

    // Creating nodes...
    NodeContainer allNodes,n01,n12;
    allNodes.Create(3);
    n01.Add(allNodes.Get(0));
    n01.Add(allNodes.Get(1));

    n12.Add(allNodes.Get(1));
    n12.Add(allNodes.Get(2));

    // Creating channel...
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("1ms"));

    // Connecting nodes, forming devices...
    NetDeviceContainer devices01,devices12;
    devices01 = p2p.Install(n01);
    devices12 = p2p.Install(n12);

    // Installing IP Stack and enabling RIP Routing...
    RipHelper ripHelper;
    InternetStackHelper IpStack;
    IpStack.SetRoutingHelper(ripHelper);
    IpStack.Install(allNodes);

    // Assigning IPv4 Addresses...
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.0.0.0", Ipv4Mask("/8"));
    ipv4.Assign(devices01);

    ipv4.SetBase("20.0.0.0", Ipv4Mask("/8"));
    ipv4.Assign(devices12);

    // Pinging from n0(10.0.0.1) to n2(20.0.0.2)...
    uint32_t packetSize = 1024;
    Time interPacketInterval = Seconds(1.0);
    PingHelper ping(Ipv4Address("20.0.0.2"));

    ping.SetAttribute("Interval", TimeValue(interPacketInterval));
    ping.SetAttribute("Size", UintegerValue(packetSize));
    ping.SetAttribute("Count", UintegerValue(5));

    ApplicationContainer apps = ping.Install(allNodes.Get(0));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));

    p2p.EnablePcapAll("basic-rip-routing");

    AnimationInterface anim("basic-rip-routing.xml");
    anim.SetConstantPosition(n01.Get(0), 10, 10);
    anim.SetConstantPosition(n01.Get(1), 0, 20);
    anim.SetConstantPosition(n12.Get(1), 20, 20);

    // Run simulation
    Simulator::Stop(Seconds(20.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}