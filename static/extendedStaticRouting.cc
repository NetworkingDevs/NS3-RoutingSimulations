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

void printRoutingTable(Ptr<Node> node, Ptr<OutputStreamWrapper> os) {
	Ipv4StaticRoutingHelper staticRoutingHelper;
  	Ptr<Ipv4StaticRouting> staticRouting = staticRoutingHelper.GetStaticRouting(node->GetObject<Ipv4>());
  	staticRouting->PrintRoutingTable (os);
}

int main() {
    // Enable logging for server and client...
    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    
    // Create  3 nodes...
    NodeContainer nodes,nodesGrpA,nodesGrpB,nodesGrpAB;
    nodes.Create(5);
    nodesGrpA.Add(nodes.Get(0));
    nodesGrpA.Add(nodes.Get(1));
    nodesGrpA.Add(nodes.Get(2));
    nodesGrpB.Add(nodes.Get(3));
    nodesGrpB.Add(nodes.Get(4));
    nodesGrpAB.Add(nodesGrpA.Get(0));
    nodesGrpAB.Add(nodesGrpB.Get(0));
    
    // Set up Point-to-Point links...
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    // Set up CSMA links...
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));
    
    // Create devices...
    NetDeviceContainer devicesGrpA,devicesGrpB,devicesGrpAB;
    devicesGrpA = csma.Install(nodesGrpA);
    devicesGrpB = csma.Install(nodesGrpB);
    devicesGrpAB = csma.Install(nodesGrpAB);
    
    // Install Internet stack
    InternetStackHelper internet;
    internet.Install(nodes);
    
    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfacesA = address.Assign(devicesGrpA);
    
    address.SetBase("192.168.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfacesB = address.Assign(devicesGrpB);
    
    address.SetBase("10.10.10.0", "255.255.255.252");
    Ipv4InterfaceContainer interfacesAB = address.Assign(devicesGrpAB);
    
    // Get the Ipv4StaticRouting instance for each node
    Ipv4StaticRoutingHelper staticRoutingHelper;
    Ptr<Ipv4StaticRouting> staticRoutingGrpANode0 = staticRoutingHelper.GetStaticRouting(nodesGrpA.Get(0)->GetObject<Ipv4>());
    Ptr<Ipv4StaticRouting> staticRoutingGrpANode1 = staticRoutingHelper.GetStaticRouting(nodesGrpA.Get(1)->GetObject<Ipv4>());
    Ptr<Ipv4StaticRouting> staticRoutingGrpBNode0 = staticRoutingHelper.GetStaticRouting(nodesGrpB.Get(0)->GetObject<Ipv4>());
    Ptr<Ipv4StaticRouting> staticRoutingGrpBNode1 = staticRoutingHelper.GetStaticRouting(nodesGrpB.Get(1)->GetObject<Ipv4>());
    
    // Configure static routes
    staticRoutingGrpANode0->AddNetworkRouteTo(Ipv4Address("192.168.2.0"), Ipv4Mask("255.255.255.0"), interfacesAB.GetAddress(1), 2);
    staticRoutingGrpBNode0->AddNetworkRouteTo(Ipv4Address("192.168.1.0"), Ipv4Mask("255.255.255.0"), interfacesAB.GetAddress(0), 2);
    staticRoutingGrpANode1->AddNetworkRouteTo(Ipv4Address("192.168.2.0"), Ipv4Mask("255.255.255.0"), interfacesA.GetAddress(0), 1);
    staticRoutingGrpANode1->AddNetworkRouteTo(Ipv4Address("10.10.10.0"), Ipv4Mask("255.255.255.252"), interfacesA.GetAddress(0), 1);
    staticRoutingGrpBNode1->AddNetworkRouteTo(Ipv4Address("192.168.1.0"), Ipv4Mask("255.255.255.0"), interfacesB.GetAddress(0), 1);
    staticRoutingGrpBNode1->AddNetworkRouteTo(Ipv4Address("10.10.10.0"), Ipv4Mask("255.255.255.252"), interfacesB.GetAddress(0), 1);
    
    // Enable packet capture
    csma.EnablePcapAll("static-routing");
    
    Ptr<OutputStreamWrapper> routingTblPrintStream = Create<OutputStreamWrapper> (&std::cout);
    Simulator::Schedule (Seconds(1.0), &printRoutingTable, nodes.Get(0), routingTblPrintStream);
    Simulator::Schedule (Seconds(1.0), &printRoutingTable, nodes.Get(1), routingTblPrintStream);
    Simulator::Schedule (Seconds(1.0), &printRoutingTable, nodes.Get(2), routingTblPrintStream);
    Simulator::Schedule (Seconds(1.0), &printRoutingTable, nodes.Get(3), routingTblPrintStream);
    Simulator::Schedule (Seconds(1.0), &printRoutingTable, nodes.Get(4), routingTblPrintStream);
    
    // Iterate over each node and print interface IDs and IP addresses
  for (NodeContainer::Iterator nodeIt = nodes.Begin (); nodeIt != nodes.End (); ++nodeIt)
  {
    Ptr<Node> node = *nodeIt;
    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
    std::cout << "Node " << node->GetId () << ":\n";

    // Loop through each interface (interface 0 is usually the loopback)
    for (uint32_t i = 0; i < ipv4->GetNInterfaces (); i++)
    {
      // Some interfaces can have multiple IP addresses
      for (uint32_t j = 0; j < ipv4->GetNAddresses (i); j++)
      {
        Ipv4InterfaceAddress ifAddr = ipv4->GetAddress (i, j);
        std::cout << "  Interface " << i << " Address: " << ifAddr.GetLocal () << "\n";
      }
    }
    std::cout << std::endl;
  }
    
    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodesGrpB.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(interfacesB.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(4));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodesGrpA.Get(1));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    AnimationInterface anim("testStatic.xml");
    anim.SetConstantPosition(nodesGrpA.Get(0), 10, 10);
    anim.SetConstantPosition(nodesGrpA.Get(1), 0, 20);
    anim.SetConstantPosition(nodesGrpA.Get(2), 20, 20);
    anim.SetConstantPosition(nodesGrpB.Get(0), 50, 10);
    anim.SetConstantPosition(nodesGrpB.Get(1), 40, 20);
    
    // Run simulation
    Simulator::Run();
    Simulator::Destroy();
    
    return 0;
}

