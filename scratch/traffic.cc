#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

#include <fstream>
#include <iostream>
#include <string>
#include <map>

using namespace ns3;

int
main(int argc, char* argv[])
{
    uint32_t numNodes = 6;
    double distance = 10.0;
    double simTime = 5.0;

    CommandLine cmd;
    cmd.AddValue("numNodes", "Number of nodes (2-30)", numNodes);
    cmd.AddValue("distance", "Distance between nodes", distance);
    cmd.AddValue("simTime", "Simulation time in seconds", simTime);
    cmd.Parse(argc, argv);

    if (numNodes < 2 || numNodes > 30)
    {
        std::cerr << "Error: numNodes must be between 2 and 30\n";
        return 1;
    }

    NodeContainer nodes;
    nodes.Create(numNodes);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211b);

    Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("999999"));

    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    for (uint32_t i = 0; i < numNodes; ++i)
    {
        Ptr<MobilityModel> mob = nodes.Get(i)->GetObject<MobilityModel>();
        mob->SetPosition(Vector(i * distance, 0.0, 0.0));
    }

    InternetStackHelper internet;
    internet.Install(nodes);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = ipv4.Assign(devices);

    for (uint32_t i = 0; i < numNodes; ++i)
    {
        std::cout << "Node " << i << " - IP: " << interfaces.GetAddress(i) << std::endl;
    }


    // P3. Traffic & Application
    uint16_t port = 9;

    // Sink application on Node 0
    Address sinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), port));
    PacketSinkHelper sinkHelper("ns3::UdpSocketFactory", sinkLocalAddress);
    ApplicationContainer sinkApp = sinkHelper.Install(nodes.Get(0));
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(simTime));

    // OnOff applications on remaining nodes sending to Node 0
    AddressValue remoteAddress(InetSocketAddress(interfaces.GetAddress(0), port));
    OnOffHelper onoff("ns3::UdpSocketFactory", Address());
    onoff.SetAttribute("Remote", remoteAddress);
    onoff.SetAttribute("PacketSize", UintegerValue(1024));
    onoff.SetAttribute("DataRate", StringValue("500kbps"));

    ApplicationContainer clientApps;
    for (uint32_t i = 1; i < numNodes; ++i)
    {
        ApplicationContainer app = onoff.Install(nodes.Get(i));

        // Staggered start times to prevent instant collisions
        double startTime = 1.0 + (i * 0.01);
        app.Start(Seconds(startTime));
        app.Stop(Seconds(simTime));
        clientApps.Add(app);
    }

    Simulator::Stop(Seconds(simTime));

    // P4. Metrics & Data Collection
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    Simulator::Run();

    monitor->CheckForLostPackets();

    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());

    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    uint64_t totalTxPackets = 0;
    uint64_t totalRxPackets = 0;
    uint64_t totalRxBytes = 0;
    double totalDelaySeconds = 0.0;

    std::cout << "\n=== FlowMonitor Per-Flow Statistics ===\n";
    for (const auto& flow : stats)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(flow.first);

        std::cout << "Flow ID: " << flow.first << "\n";
        std::cout << "  Source:      " << t.sourceAddress << "\n";
        std::cout << "  Destination: " << t.destinationAddress << "\n";
        std::cout << "  Tx Packets:  " << flow.second.txPackets << "\n";
        std::cout << "  Rx Packets:  " << flow.second.rxPackets << "\n";
        std::cout << "  Rx Bytes:    " << flow.second.rxBytes << "\n";
        std::cout << "  Delay Sum:   " << flow.second.delaySum.GetSeconds() << " s\n";

        totalTxPackets += flow.second.txPackets;
        totalRxPackets += flow.second.rxPackets;
        totalRxBytes += flow.second.rxBytes;
        totalDelaySeconds += flow.second.delaySum.GetSeconds();
    }

    double throughputKbps = 0.0;
    double avgDelayMs = 0.0;
    double lossRatioPercent = 0.0;

    if (simTime > 0.0)
    {
        throughputKbps = (totalRxBytes * 8.0) / simTime / 1000.0;
    }

    if (totalRxPackets > 0)
    {
        avgDelayMs = (totalDelaySeconds / totalRxPackets) * 1000.0;
    }

    if (totalTxPackets > 0)
    {
        lossRatioPercent = ((double)(totalTxPackets - totalRxPackets) / (double)totalTxPackets) * 100.0;
    }

    std::cout << "\n=== Summary Statistics ===\n";
    std::cout << "Number of nodes: " << numNodes << "\n";
    std::cout << "Total Tx Packets: " << totalTxPackets << "\n";
    std::cout << "Total Rx Packets: " << totalRxPackets << "\n";
    std::cout << "Total Rx Bytes: " << totalRxBytes << "\n";
    std::cout << "Throughput: " << throughputKbps << " Kbps\n";
    std::cout << "Average Delay: " << avgDelayMs << " ms\n";
    std::cout << "Loss Ratio: " << lossRatioPercent << " %\n";

    // Append results to summary.csv
    // Format: numNodes,txPackets,rxPackets,rxBytes,throughputKbps,avgDelayMs,lossRatioPercent
    std::ifstream checkFile("scratch/my-wifi/summary.csv");
    bool writeHeader = false;

    if (!checkFile.good())
    {
        writeHeader = true;
    }
    else
    {
        checkFile.seekg(0, std::ios::end);
        if (checkFile.tellg() == 0)
        {
            writeHeader = true;
        }
    }
    checkFile.close();

    std::ofstream outFile("scratch/my-wifi/summary.csv", std::ios::app);
    if (outFile.is_open())
    {
        if (writeHeader)
        {
            outFile << "numNodes,txPackets,rxPackets,delaySumSeconds,rxBytes,throughputKbps,avgDelayMs,lossRatioPercent\n";
        }

        outFile << numNodes << ","
                << totalTxPackets << ","
                << totalRxPackets << ","
                << totalDelaySeconds << ","
                << totalRxBytes << ","
                << throughputKbps << ","
                << avgDelayMs << ","
                << lossRatioPercent << "\n";

        outFile.close();
    }
    else
    {
        std::cerr << "Error: could not open summary.csv for writing\n";
    }

    Simulator::Destroy();
    return 0;
}

