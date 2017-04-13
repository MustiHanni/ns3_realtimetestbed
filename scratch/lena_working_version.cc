/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
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
 *
 * Author: Jaume Nin <jaume.nin@cttc.cat>
 */

//root@left:# route add default gw 1.0.1.1 eth0					root@right:# route add default gw 1.0.2.1 eth0
//root@left:# ip route add 1.0.0.0/24 via 1.0.1.1 src 1.0.1.2 	root@right:# ip route add 7.0.0.0/8 via 1.0.2.1 src 1.0.2.2
//root@left:# ip route add 7.0.0.0/8 via 1.0.1.1 src 1.0.1.2 	root@right:# ip route add 1.0.0.0/24 via 1.0.2.1 src 1.0.2.2
//root@left:# ip route add 1.0.2.0/24 via 1.0.1.1 src 1.0.1.2 	root@right:# ip route add 1.0.1.0/24 via 1.0.2.1 src 1.0.2.2
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "ns3/csma-module.h"
#include "ns3/tap-bridge-module.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/ipv4-list-routing.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/lte-module.h"
#include "ns3/config-store.h"
#include "ns3/lte-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
//#include "ns3/gtk-config-store.h"

using namespace ns3;

/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeB,
 * attaches one UE per eNodeB starts a flow for each UE to  and from a remote host.
 * It also  starts yet another flow between each UE pair.
 */

NS_LOG_COMPONENT_DEFINE ("LenaTest");

void setTraceConfig();
static void VNetD_MacTx(std::string path, Ptr<const Packet> packet);
static void VNetD_MacRx(std::string path, Ptr<const Packet> packet);
static void VNetD_MacPromiscRx(std::string path, Ptr<const Packet> packet);
//void PrintRoutingTable (Ptr<Node>& n);
static void myTraceCallback (std::string path, Ptr<const Packet> packet, Ptr<Ipv4> ipv4h,  uint32_t interface);
static void csmaTraceTx(std::string path, Ptr< const Packet > packet);
static void csmaTraceRx(std::string path, Ptr< const Packet > packet);

static void p2pTraceTx(std::string path, Ptr< const Packet > packet);
static void p2pTraceRx(std::string path, Ptr< const Packet > packet);

static void tracePDCPtx(std::string path, uint16_t rnti, uint8_t lcid, uint32_t pSize);
static void tracePDCPrx(std::string path, uint16_t rnti, uint8_t lcid, uint32_t pSize, uint64_t  delayNanoSeconds );
static void traceRLCtx(std::string path, uint16_t rnti, uint8_t lcid, uint32_t pSize);
static void traceRLCrx(std::string path, uint16_t rnti, uint8_t lcid, uint32_t pSize, uint64_t  delayNanoSeconds );

static void UEtraceRRCrandomAccessSuccessful(std::string path, uint64_t imsi, uint16_t cellid, uint16_t rnti);
static void UEtraceRRCconnectionEstablished(std::string path, uint64_t imsi, uint16_t cellid, uint16_t rnti);
static void UEtraceRRCconnectionReconfiguration(std::string path, uint64_t imsi, uint16_t cellid, uint16_t rnti);

static void eNBtraceDLtx(std::string path, Ptr<const PacketBurst> packets);
static void eNBtraceULrx(std::string path, Ptr<const Packet> packet);
static void UEtraceDLrx(std::string path, Ptr<const Packet> packet);
static void UEtraceULtx(std::string path, Ptr<const PacketBurst> packets);
void show_Ip_Addr_Inface(Ptr<Node> remoteHost,Ptr<Node> pgw,NodeContainer ueNodes,NodeContainer enbNodes);
int
main (int argc, char *argv[])
{
  uint16_t numberOfNodes = 1;
  double simTime = 60;
  double distance = 60.0;
  double maxNumOfPackets = 1;
  double interPacketInterval =100;
  bool enableApplication = false;
  bool schowIPinterface = false;

  // Command line arguments
  CommandLine cmd;

  // We are interacting with the outside, world. we have to use the real-time simulator and calculate checksums.
  GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));

  cmd.AddValue("numberOfNodes", "Number of eNodeBs + UE pairs", numberOfNodes);
  cmd.AddValue("simTime", "Total duration of the simulation [s])", simTime);
  cmd.AddValue("distance", "Distance between eNBs [m]", distance);
  cmd.AddValue("maxNumOfPackets", "number of maximunm packets)", maxNumOfPackets);
  cmd.AddValue("interPacketInterval", "Inter packet interval [ms])", interPacketInterval);
  cmd.AddValue("enableApplication", "install clientSever App)", enableApplication);
  cmd.AddValue("schowIPinterface", "enable Tracing)", schowIPinterface);
  cmd.Parse(argc, argv);

   // Create a single RemoteHost
  NodeContainer ghLeft,remoteHostContainer;
  ghLeft.Create(1);
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults();

  // parse again so you can override default values from the command line
  cmd.Parse(argc, argv);

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("1.0.1.0"), Ipv4Mask ("255.255.255.0"), 2);//remoteHostStaticRouting->AddHostRouteTo    (Ipv4Address ("1.0.1.2"), Ipv4Address ("1.0.1.1"), 2);
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("1.0.0.0"), Ipv4Mask ("255.255.255.0"), 1);
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0")    ,Ipv4Address ("1.0.0.1"), 1);
  remoteHostStaticRouting->AddHostRouteTo    (Ipv4Address ("7.0.0.1"), Ipv4Address ("1.0.0.1"), 1);
  remoteHostStaticRouting->AddHostRouteTo    (Ipv4Address ("7.0.0.2"), Ipv4Address ("1.0.0.1"), 1);
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("1.0.2.0"), Ipv4Mask ("255.255.255.0"),Ipv4Address ("1.0.0.1"), 1);
  remoteHostStaticRouting->AddHostRouteTo    (Ipv4Address ("1.0.2.1"), Ipv4Address ("1.0.0.1"), 1);
  remoteHostStaticRouting->AddHostRouteTo    (Ipv4Address ("1.0.2.2"), Ipv4Address ("1.0.0.1"), 1);

  NodeContainer enbNodes;
  NodeContainer ueNodes;
  NodeContainer ghRight;
  enbNodes.Create(numberOfNodes);
  ueNodes.Create(numberOfNodes);
  ghRight.Create(1);

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < numberOfNodes; i++)
    {
      positionAlloc->Add (Vector(distance * i, 0, 0));
    }
  MobilityHelper mobility;
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(enbNodes);
  mobility.Install(ueNodes);

  // Install LTE Devices to the nodes
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
  // Assign IP address to UEs, and install applications

  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  // Attach one UE per eNodeB
  for (uint16_t i = 0; i < numberOfNodes; i++)
      {
        lteHelper->Attach (ueLteDevs.Get(i), enbLteDevs.Get(i));
        // side effect: the default EPS bearer will be activated
      }
  // Activate an EPS bearer on all UEs

//  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
//    {
      /*Ptr<NetDevice> ueDevice = ueLteDevs.Get (0);
      //Ptr<NetDevice> ueDevice = ueLteDevs.Get (u);
      GbrQosInformation qos;
      qos.gbrDl = 132;  // bit/s, considering IP, UDP, RLC, PDCP header size
      qos.gbrUl = 132;
      qos.mbrDl = qos.gbrDl;
      qos.mbrUl = qos.gbrUl;
      enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
      EpsBearer bearer (q, qos);
      //bearer.arp.priorityLevel = 15 - (u + 1);
      bearer.arp.priorityLevel = 15 - (0 + 1);
      bearer.arp.preemptionCapability = true;
      bearer.arp.preemptionVulnerability = true;*/
      Ptr<EpcTft> tft = Create<EpcTft> ();
      EpcTft::PacketFilter pf;
      pf.localPortStart = 1234;
      pf.localPortEnd = 1234;
      tft->Add (pf);
      //lteHelper->ActivateDedicatedEpsBearer (ueDevice, bearer, EpcTft::Default ());
      lteHelper->ActivateDedicatedEpsBearer (ueLteDevs, EpsBearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT), tft);
//    }
  //--------------------------------------------------------------------------------------
  CsmaHelper csmaLeft;
  csmaLeft.SetChannelAttribute ("DataRate", DataRateValue (DataRate("1000Gb/s")));
  csmaLeft.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  csmaLeft.SetChannelAttribute ("Delay", TimeValue (Seconds (0.0)));
  NetDeviceContainer DevicesLeft = csmaLeft.Install (NodeContainer(remoteHost,ghLeft.Get(0)));
  CsmaHelper csmaRight;
  csmaRight.SetChannelAttribute ("DataRate", DataRateValue (DataRate("1000Gb/s")));
  csmaRight.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  csmaRight.SetChannelAttribute ("Delay", TimeValue (Seconds (0.0)));
  NetDeviceContainer DevicesRight = csmaRight.Install (NodeContainer(ueNodes.Get (0),ghRight.Get(0)));

  Ipv4AddressHelper ipv4ghNetLeft,ipv4ghNetRight;
  ipv4ghNetLeft.SetBase ("1.0.1.0", "255.255.255.0");
  Ipv4InterfaceContainer IpIfacesLeft = ipv4ghNetLeft.Assign(DevicesLeft.Get(0));
  ipv4ghNetRight.SetBase ("1.0.2.0", "255.255.255.0");
  Ipv4InterfaceContainer IpIfacesRight = ipv4ghNetRight.Assign(DevicesRight.Get(0));

  Ptr<Ipv4StaticRouting> pgwStaticRouting = ipv4RoutingHelper.GetStaticRouting (pgw->GetObject<Ipv4> ());
  pgwStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"),Ipv4Mask("255.0.0.0"),1);
  pgwStaticRouting->AddNetworkRouteTo(Ipv4Address("1.0.2.0"),Ipv4Mask("255.255.255.0"),Ipv4Address("7.0.0.2"),1);
  pgwStaticRouting->AddHostRouteTo(Ipv4Address("1.0.2.1"),Ipv4Address("7.0.0.2"),1);
  pgwStaticRouting->AddHostRouteTo(Ipv4Address("1.0.2.2"),Ipv4Address("7.0.0.2"),1);

  pgwStaticRouting->AddNetworkRouteTo(Ipv4Address("1.0.0.0"),Ipv4Mask("255.255.255.0"),2);
  pgwStaticRouting->AddNetworkRouteTo(Ipv4Address("1.0.1.0"),Ipv4Mask("255.255.255.0"),Ipv4Address("1.0.0.2"),2);
  pgwStaticRouting->AddHostRouteTo(Ipv4Address("1.0.1.1"),Ipv4Address("1.0.0.2"),2);
  pgwStaticRouting->AddHostRouteTo(Ipv4Address("1.0.1.2"),Ipv4Address("1.0.0.2"),2);

  Ptr<Ipv4StaticRouting> ueStaticRoutingEx = ipv4RoutingHelper.GetStaticRouting (ueNodes.Get (0)->GetObject<Ipv4> ());
  ueStaticRoutingEx->AddNetworkRouteTo(Ipv4Address("1.0.2.0"),Ipv4Mask("255.255.255.0"),2);
  ueStaticRoutingEx->AddNetworkRouteTo(Ipv4Address("7.0.0.0"),Ipv4Mask("255.0.0.0"),1);
  ueStaticRoutingEx->AddNetworkRouteTo(Ipv4Address("1.0.0.0"),Ipv4Mask("255.255.255.0"),Ipv4Address("7.0.0.1"),1);
  pgwStaticRouting->AddHostRouteTo(Ipv4Address("1.0.0.1"),Ipv4Address("7.0.0.1"),1);
  pgwStaticRouting->AddHostRouteTo(Ipv4Address("1.0.0.2"),Ipv4Address("7.0.0.1"),1);
  ueStaticRoutingEx->AddNetworkRouteTo(Ipv4Address("1.0.1.0"),Ipv4Mask("255.255.255.0"),Ipv4Address("7.0.0.1"),1);
  pgwStaticRouting->AddHostRouteTo(Ipv4Address("1.0.1.1"),Ipv4Address("7.0.0.1"),1);
  pgwStaticRouting->AddHostRouteTo(Ipv4Address("1.0.1.2"),Ipv4Address("7.0.0.1"),1);

  //------------------------------------------------- Tap Devices
  TapBridgeHelper tapBridge;
  tapBridge.SetAttribute ("Mode", StringValue ("UseBridge"));
  //tapBridge.SetAttribute ("Mode", StringValue ("UseLocal"));
  tapBridge.SetAttribute ("DeviceName", StringValue ("tap-left"));
  tapBridge.Install (ghLeft.Get(0), DevicesLeft.Get (1));/**/

  tapBridge.SetAttribute ("DeviceName", StringValue ("tap-right"));
  tapBridge.Install (ghRight.Get(0), DevicesRight.Get(1));
  //-------------------------------------------------------------
  if(enableApplication == true){
  uint16_t dlPort = 1234;
  uint16_t ulPort = 2000;
  uint16_t otherPort = 3000;
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)    {
      ++ulPort;
      ++otherPort;
      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
      PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
      PacketSinkHelper   packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
      serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get(0)));//serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get(u)));
      serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
      serverApps.Add (  packetSinkHelper.Install (ueNodes.Get(0)));//serverApps.Add (packetSinkHelper.Install (ueNodes.Get(u)));

      UdpClientHelper dlClient (ueIpIface.GetAddress (0), dlPort);//UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
      dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      dlClient.SetAttribute ("MaxPackets", UintegerValue(maxNumOfPackets));

      UdpClientHelper ulClient (remoteHostAddr, ulPort);
      ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      ulClient.SetAttribute ("MaxPackets", UintegerValue(maxNumOfPackets));

      UdpClientHelper client (ueIpIface.GetAddress (0), otherPort);//UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
      client.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      client.SetAttribute ("MaxPackets", UintegerValue(maxNumOfPackets));

      clientApps.Add (dlClient.Install (remoteHost));
      clientApps.Add (ulClient.Install (ueNodes.Get(0)));//clientApps.Add (ulClient.Install (ueNodes.Get(u)));
/*      if (u+1 < ueNodes.GetN ())
          clientApps.Add (client.Install (ueNodes.Get(u+1)));
      else*/
          clientApps.Add (client.Install (ueNodes.Get(0)));
    }
  serverApps.Start (Seconds(1));
  clientApps.Start (Seconds(1));
  }
  lteHelper->EnableTraces ();
  // Uncomment to enable PCAP tracing
  p2ph.EnablePcapAll("lena-test");
  //csmaLeft.EnablePcapAll("csmaleft");
//  LogComponentEnable ("EpcSgwPgwApplication", LOG_LEVEL_ALL);
//  LogComponentEnable ("LteNetDevice", LOG_LEVEL_ALL);
//  lteHelper->EnableLogComponents ();
  Packet::EnablePrinting ();

  //GtkConfigStore config;
  //config.ConfigureAttributes();
  if(schowIPinterface)
	  show_Ip_Addr_Inface(remoteHost,pgw,ueNodes,enbNodes);
  //PrintRoutingTable (pgw);
  setTraceConfig();

  Simulator::Stop(Seconds(simTime));
  Simulator::Run();
  Simulator::Destroy();
  return 0;

}

void setTraceConfig(){

	  if(0){
		  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::VirtualNetDevice/MacTx", MakeCallback (&VNetD_MacTx));
		  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::VirtualNetDevice/MacRx", MakeCallback (&VNetD_MacRx));
		  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::VirtualNetDevice/MacPromiscRx", MakeCallback (&VNetD_MacPromiscRx));
	      Config::Connect ("/NodeList/1/$ns3::Ipv4L3Protocol/Tx", MakeCallback (&myTraceCallback));
	      Config::Connect ("/NodeList/*/DeviceList/*/$ns3::CsmaNetDevice/PhyTxEnd",MakeCallback (&csmaTraceTx));
	      Config::Connect ("/NodeList/*/DeviceList/*/$ns3::CsmaNetDevice/PhyRxEnd",MakeCallback (&csmaTraceRx));

	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/PhyTxEnd",MakeCallback (&p2pTraceTx));
	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/PhyRxEnd",MakeCallback (&p2pTraceRx));

	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteRlc/TxPDU",MakeCallback (&traceRLCtx));
	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteRlc/RxPDU",MakeCallback (&traceRLCrx));
	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LtePdcp/TxPDU",MakeCallback (&tracePDCPtx));
	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LtePdcp/RxPDU",MakeCallback (&tracePDCPrx));

	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteNetDevice/$ns3::LteUeNetDevice/LteUeRrc/RandomAccessSuccessful",  MakeCallback (&UEtraceRRCrandomAccessSuccessful));
	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteNetDevice/$ns3::LteUeNetDevice/LteUeRrc/ConnectionEstablished",  MakeCallback (&UEtraceRRCconnectionEstablished));
	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteNetDevice/$ns3::LteUeNetDevice/LteUeRrc/ConnectionReconfiguration",  MakeCallback (&UEtraceRRCconnectionReconfiguration));

	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteEnbNetDevice/LteEnbPhy/DlSpectrumPhy/TxEnd",  MakeCallback (&eNBtraceDLtx));
	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteEnbNetDevice/LteEnbPhy/UlSpectrumPhy/RxEndOk",  MakeCallback (&eNBtraceULrx));
	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/LteUePhy/DlSpectrumPhy/RxEndOk",  MakeCallback (&UEtraceDLrx));
	  Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/LteUePhy/UlSpectrumPhy/TxEnd",  MakeCallback (&UEtraceULtx));
	  }
}
static void VNetD_MacTx(std::string path, Ptr<const Packet> packet){
	std::cout<<"VNetD_MacTx"<<std::endl;
}
static void VNetD_MacRx(std::string path, Ptr<const Packet> packet){
	std::cout<<"VNetD_MacRx"<<std::endl;
}
static void VNetD_MacPromiscRx(std::string path, Ptr<const Packet> packet){
	std::cout<<"VNetD_MacPromiscRx"<<std::endl;
}
static void myTraceCallback (std::string path, Ptr<const Packet> packet, Ptr<Ipv4> ipv4h,  uint32_t interface){
  //Vector position = model->GetPosition ();
  std::cout << "Time: " << Simulator::Now().GetSeconds() <<"| Node: " << path.substr(10,1) <<std::endl;
  Ptr<Packet> p = packet->Copy ();
  std::cout  << *p << std::endl;
  std::cout << "----------------------------------------------------"<<std::endl;
}
static void csmaTraceTx(std::string path, Ptr< const Packet > packet){
	if(packet==NULL){
	}else{
		std::cout<< "csma PHY Tx at Time: "<< std::setw(10) << Simulator::Now().GetSeconds() <<"| Node("<< path.substr(10,1)<<")" <<std::endl;
	}
}
static void csmaTraceRx(std::string path, Ptr< const Packet > packet){
	if(packet==NULL){
	}else{
		std::cout<< "csma PHY Rx at Time: "<< std::setw(10) << Simulator::Now().GetSeconds() <<"| Node("<< path.substr(10,1)<<")" <<std::endl;
	}
}
static void p2pTraceTx(std::string path, Ptr< const Packet > packet){
	if(packet==NULL){
	}else{
		std::cout<< "p2p PHY Tx at Time: "<< std::setw(10) << Simulator::Now().GetSeconds() <<"| Node("<< path.substr(10,1)<<")" <<std::endl;
	}
}
static void p2pTraceRx(std::string path, Ptr< const Packet > packet){
	if(packet==NULL){
	}else{
		std::cout<< "p2p PHY Rx at Time: "<< std::setw(10) << Simulator::Now().GetSeconds() <<"| Node("<< path.substr(10,1)<<")" <<std::endl;
	}
}

static void tracePDCPtx(std::string path, uint16_t rnti, uint8_t lcid, uint32_t pSize){
	  std::cout << "Time: " << Simulator::Now().GetSeconds() <<"| Node: " << path.substr(10,1) <<" Tx-PDCP"<<std::endl;
	  std::cout << "rnti: " << rnti <<"| lcid: " << lcid <<"| pSize: "<< pSize <<std::endl;
	  std::cout << "----------------------------------------------------"<<std::endl;
}
static void tracePDCPrx(std::string path, uint16_t rnti, uint8_t lcid, uint32_t pSize, uint64_t  delayNanoSeconds ){
	  std::cout << "Time: " << Simulator::Now().GetSeconds() <<"| Node: " << path.substr(10,1) <<" Rx-PDCP"<<std::endl;
	  std::cout << "rnti: " << rnti <<"| lcid: " << lcid <<"| pSize: "<< pSize <<"| delayNanoSeconds: "<< delayNanoSeconds <<std::endl;
	  std::cout << "----------------------------------------------------"<<std::endl;
}
static void traceRLCtx(std::string path, uint16_t rnti, uint8_t lcid, uint32_t pSize){
	  std::cout << "traceRLCtx"<<std::endl;
}
static void traceRLCrx(std::string path, uint16_t rnti, uint8_t lcid, uint32_t pSize, uint64_t  delayNanoSeconds ){
	  std::cout << "traceRLCrx"<<std::endl;
}

static void UEtraceRRCrandomAccessSuccessful(std::string path, uint64_t imsi, uint16_t cellid, uint16_t rnti){
	  std::cout << "RRC-randomAccess"<<std::endl;
}
static void UEtraceRRCconnectionEstablished(std::string path, uint64_t imsi, uint16_t cellid, uint16_t rnti){
	  std::cout << "RRC-connectionEstablisched"<<std::endl;
}
static void UEtraceRRCconnectionReconfiguration(std::string path, uint64_t imsi, uint16_t cellid, uint16_t rnti){
	  std::cout << "RRC-connectionRecon"<<std::endl;
}

static void eNBtraceDLtx(std::string path, Ptr<const PacketBurst> packets){
	if(packets==NULL){
	}else{
		std::cout<< "eNBPhDL Tx at Time: " << std::setw(10) << Simulator::Now().GetSeconds() <<"| Node("
				<< path.substr(10,1)<<")"<<" Total packet " <<packets->GetNPackets()<<std::endl;
	}
}
static void eNBtraceULrx(std::string path, Ptr<const Packet> packet){
	if(packet==NULL){
	}else{
		std::cout<< "eNBPyUL Rx at Time: " << std::setw(10) << Simulator::Now().GetSeconds() <<"| Node(" << path.substr(10,1)<<")"<<std::endl;
	}
}
static void UEtraceDLrx(std::string path, Ptr<const Packet> packet){
	if(packet==NULL){
	}else{
		std::cout<< "UEPhyDL Rx at Time: " << std::setw(10) << Simulator::Now().GetSeconds() <<"| Node("
				<< path.substr(10,1)<<")"<<std::endl;
	}
}
static void UEtraceULtx(std::string path, Ptr<const PacketBurst> packets){
	if(packets==NULL){
	}else{
		std::cout<< "UEPhyUL Tx at Time: " << std::setw(10) << Simulator::Now().GetSeconds() <<"| Node("
				<< path.substr(10,1)<<")"<<" Total packet " <<packets->GetNPackets()<<std::endl;
	}
}
/**
 * \brief Print the routing table.
 * \param n the node
 */
/*void PrintRoutingTable (Ptr<Node>& n){
  Ptr<Ipv4StaticRouting> routing = 0;
  Ipv4StaticRoutingHelper routingHelper;
  Ptr<Ipv4> ipv4 = n->GetObject<Ipv4> ();
  Ipv4RoutingTableEntry route;
  routing = routingHelper.GetStaticRouting (ipv4);
  std::cout << "Routing table of " << n << " : " << std::endl;
  std::cout << std::setw(12)<< "Destination" << std::setw(10) << "Gateway" << std::setw(10)<< "Interface" << std::endl;
  for (uint32_t i = 0; i < routing->GetNRoutes (); i++)
    {
      route = routing->GetRoute (i);
      std::cout << route.GetDest () //<< "\t"
				<< std::setw(10) << route.GetGateway ()// << "\t"
				<< std::setw(10) << route.GetInterface ()// << "\t"
                << std::endl;
    }
}*/

void show_Ip_Addr_Inface(Ptr<Node> remoteHost,Ptr<Node> pgw,NodeContainer ueNodes,NodeContainer enbNodes){
	  Ptr<Ipv4> iph;
	  Ipv4Address addr;
	  std::cout <<"=== Node IP Address & Interface =="<<std::endl;
	  iph = remoteHost->GetObject<Ipv4>();
	  addr = iph->GetAddress (1, 0).GetLocal ();
	  std::cout<<"RHo-ID: "<<remoteHost->GetId() <<" RHo Addr : "<<addr <<" | Interface : 1"<< std::endl;
	  addr = iph->GetAddress (2, 0).GetLocal ();
	  std::cout<<"RHo-ID: "<<remoteHost->GetId() <<" RHo Addr : "<<addr <<" | Interface : 2"<< std::endl;

	  iph = pgw->GetObject<Ipv4> ();
	  addr = iph->GetAddress (1, 0).GetLocal ();
	  std::cout<<"PGW-ID: "<<pgw->GetId() <<" PGW Addr : "<<addr <<" | Interface : 1"<< std::endl;
	  addr = iph->GetAddress (2, 0).GetLocal ();
	  std::cout<<"PGW-ID: "<<pgw->GetId() <<" PGW Addr : "<<addr <<" | Interface : 2"<< std::endl;
	  addr = iph->GetAddress (3, 0).GetLocal ();
	  std::cout<<"PGW-ID: "<<pgw->GetId() <<" PGW Addr : "<<addr <<"| Interface : 3"<< std::endl;

	  iph = enbNodes.Get(0)->GetObject<Ipv4> ();
	  addr = iph->GetAddress (1, 0).GetLocal ();
	  std::cout<<"eNB-ID: "<<(enbNodes.Get(0))->GetId() <<" enB Addr : "<<addr <<"| Interface : 1"<< std::endl;

	  iph = ueNodes.Get(0)->GetObject<Ipv4> ();
	  addr = iph->GetAddress (1, 0).GetLocal ();
	  std::cout<<" UE-ID: "<<(ueNodes.Get(0))->GetId() <<" UE  Addr : "<<addr <<" | Interface : 1"<< std::endl;
	  addr = iph->GetAddress (2, 0).GetLocal ();
	  std::cout<<" UE-ID: "<<(ueNodes.Get(0))->GetId() <<" UE  Addr : "<<addr <<" | Interface : 2"<< std::endl;
	  std::cout <<"=================================="<<std::endl;
}
