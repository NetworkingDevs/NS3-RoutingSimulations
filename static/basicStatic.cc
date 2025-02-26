/**
 * Learn to simulate the static routing in NS3.
 * This file includes the code to simulate st-
 * atic routing which will teach you that how
 * you can add static routes and all that stuff.
 **/
// For General Imports
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
// For Routing...
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
// For client and server communication...
#include "ns3/applications-module.h"
// For Netanim 
#include "ns3/netanim-module.h"

using namespace ns3;

int main() {
    // Enable logging for server and client...
    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    
    // Create  3 nodes...
    NodeContainer nodes,nodesGrpAB,nodesGrpBC;
    nodes.Create(3);
    nodesGrpAB.Add(nodes.Get(0));
    nodesGrpAB.Add(nodes.Get(1));
    nodesGrpBC.Add(nodes.Get(1));
    nodesGrpBC.Add(nodes.Get(2));
    
    // Set up Point-to-Point links...
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    
    // Create devices...
    NetDeviceContainer devicesGrpAB,devicesGrpBC;
    devicesGrpAB = p2p.Install(nodesGrpAB);
    devicesGrpBC = p2p.Install(nodesGrpBC);
    
    // Install Internet stack
    InternetStackHelper internet;
    internet.Install(nodes);
    
    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfacesAB = address.Assign(devicesGrpAB);
    
    address.SetBase("192.168.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfacesBC = address.Assign(devicesGrpBC);
    
    // Get the Ipv4StaticRouting instance for each node
    Ipv4StaticRoutingHelper staticRoutingHelper;
    Ptr<Ipv4StaticRouting> staticRoutingGrpABNode0 = staticRoutingHelper.GetStaticRouting(nodesGrpAB.Get(0)->GetObject<Ipv4>());
    Ptr<Ipv4StaticRouting> staticRoutingGrpBCNode1 = staticRoutingHelper.GetStaticRouting(nodesGrpBC.Get(1)->GetObject<Ipv4>());
    
    // Configure static routes
    staticRoutingGrpABNode0->AddNetworkRouteTo(Ipv4Address("192.168.2.0"), Ipv4Mask("255.255.255.0"), interfacesAB.GetAddress(1), 1);
    
    // comment below line and observe the changes...
    staticRoutingGrpBCNode1->AddNetworkRouteTo(Ipv4Address("192.168.1.0"), Ipv4Mask("255.255.255.0"), interfacesBC.GetAddress(0), 1);
    
    // Enable packet capture
    p2p.EnablePcapAll("basic-static-routing");
    
    // Client and Server Communication...
    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodesGrpBC.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(interfacesBC.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(4));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodesGrpAB.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    // for NetAnim...
    AnimationInterface anim("basic-static-routing.xml");
    anim.SetConstantPosition(nodesGrpAB.Get(0), 10, 10);
    anim.SetConstantPosition(nodesGrpAB.Get(1), 0, 20);
    anim.SetConstantPosition(nodesGrpBC.Get(1), 20, 20);
    
    // Run simulation
    Simulator::Run();
    Simulator::Destroy();
    
    return 0;
}

