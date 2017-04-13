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
//
//
//root@left:# route add default gw 1.0.1.1 eth0					root@right:# route add default gw 1.0.2.1 eth0
//root@left:# ip route add 1.0.0.0/24 via 1.0.1.1 src 1.0.1.2 	root@right:# ip route add 7.0.0.0/8 via 1.0.2.1 src 1.0.2.2
//root@left:# ip route add 7.0.0.0/8 via 1.0.1.1 src 1.0.1.2 	root@right:# ip route add 1.0.0.0/24 via 1.0.2.1 src 1.0.2.2
//root@left:# ip route add 1.0.2.0/24 via 1.0.1.1 src 1.0.1.2 	root@right:# ip route add 1.0.1.0/24 via 1.0.2.1 src 1.0.2.2
//
//   iperf -s															           iperf -c 1.0.1.2
//   _________																	       __________
//  |   lxc   |																	      |   lxc    |
//  |Container| 																      |Container |
//  |   left  |																	      |  right   |
//  |         |																	      |          |
//  |  eth0   |																	      |   eth0   |
//  | 1.0.1.2 |										     lte					      |  1.0.2.2 |
//  |---------|								((|))       Radio   	     		      |----------|
//  | TapLeft |								  |                     |__	        	  | TapRight |
//	|---------|	(1.0.1.0)					 / \                    |  |  (1.0.2.0)   |----------|
//  |LEFT_Node____csma____PGW<------lte------eNB                    |UE|_____csma______RIGHT_Node|
//		 			  	   |				 (10.0.0.6)		         |
//		 			  	   v				            	         v
//                (1.0.1.1)|(7.0.0.1)                       (7.0.0.2)|(1.0.2.1)
//
//
//

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include "ns3/network-module.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/tap-bridge-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/ipv4-list-routing.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/lte-helper.h"
#include "ns3/lte-module.h"
#include "ns3/config-store.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TapLTE");

int 
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  // We are interacting with the outside, world. we have to use the real-time simulator and calculate checksums.
  GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));

  //parse again so you can override default values from the command line
  cmd.Parse(argc, argv);

  //Create LTE Helper and install all LTE stuff
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  lteHelper->SetSchedulerType("ns3::RrFfMacScheduler");

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults();

  // Create two ghost nodes.
  NodeContainer enbNodes,ueNodes ,ghLeft,ghRight;
  ghLeft.Create(1);
  ghRight.Create(1);
  enbNodes.Create(1);
  ueNodes.Create(1);

  // Install the IP stack on the UEs
  InternetStackHelper internet;
  internet.Install (ueNodes);

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector( 0, 0, 0));
  MobilityHelper mobility;
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(enbNodes);
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(ueNodes);

  // Install LTE Devices in eNB and UEs
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate("1000Gb/s")));
  csma.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  csma.SetChannelAttribute ("Delay", TimeValue (Seconds (0.001)));
  NetDeviceContainer DevicesLeft = csma.Install (NodeContainer(pgw,ghLeft.Get(0)));
  NetDeviceContainer DevicesRight = csma.Install (NodeContainer(ueNodes.Get (0),ghRight.Get(0)));

  Ipv4AddressHelper ipv4ghNetLeft,ipv4ghNetRight;
  ipv4ghNetLeft.SetBase ("1.0.1.0", "255.255.255.0");
  Ipv4InterfaceContainer IpIfacesLeft = ipv4ghNetLeft.Assign(DevicesLeft.Get(0));
  ipv4ghNetRight.SetBase ("1.0.2.0", "255.255.255.0");
  Ipv4InterfaceContainer IpIfacesRight = ipv4ghNetRight.Assign(DevicesRight.Get(0));

  Ipv4StaticRoutingHelper ipv4RoutingHelper;

  Ptr<Ipv4StaticRouting> pgwStaticRouting = ipv4RoutingHelper.GetStaticRouting (pgw->GetObject<Ipv4> ());
  pgwStaticRouting->AddNetworkRouteTo(Ipv4Address("1.0.2.0"),Ipv4Mask("255.255.255.0"),Ipv4Address("7.0.0.2"),1);
  pgwStaticRouting->AddNetworkRouteTo(Ipv4Address("1.0.1.0"),Ipv4Mask("255.255.255.0"),3);

  Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNodes.Get (0)->GetObject<Ipv4> ());
  ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
  ueStaticRouting->AddNetworkRouteTo(Ipv4Address("1.0.1.0"),Ipv4Mask("255.255.255.0"),Ipv4Address("7.0.0.1"),1);
  ueStaticRouting->AddNetworkRouteTo(Ipv4Address("1.0.2.0"),Ipv4Mask("255.255.255.0"),2);

  // Attach all UEs to the first eNodeB
  lteHelper->Attach (ueLteDevs, enbLteDevs.Get(0));

  ///***************************************** Tap Devices  ***********************************************************///
  TapBridgeHelper tapBridge;
  tapBridge.SetAttribute ("Mode", StringValue ("UseBridge"));
  //tapBridge.SetAttribute ("Mode", StringValue ("UseLocal"));
  tapBridge.SetAttribute ("DeviceName", StringValue ("tap-left"));
  tapBridge.Install (ghLeft.Get(0), DevicesLeft.Get (1));

  tapBridge.SetAttribute ("DeviceName", StringValue ("tap-right"));
  tapBridge.Install (ghRight.Get(0), DevicesRight.Get(1));

//  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
//  EpsBearer bearer (q);
//  lteHelper->ActivateDataRadioBearer (ueLteDevs, bearer);

  lteHelper->EnableTraces ();

  Ptr<Ipv4> ipv4h;
  Ipv4Address addr;
  std::cout <<"=== Node IP Address & Interface =="<<std::endl;
  ipv4h = pgw->GetObject<Ipv4> ();
  addr = ipv4h->GetAddress (1, 0).GetLocal ();
  std::cout <<"PGW Addr : "<<addr <<" | Interface : 1"<< std::endl;
  addr = ipv4h->GetAddress (2, 0).GetLocal ();
  std::cout <<"PGW Addr : "<<addr <<"| Interface : 2"<< std::endl;
  addr = ipv4h->GetAddress (3, 0).GetLocal ();
  std::cout <<"PGW Addr : "<<addr <<" | Interface : 3"<< std::endl;
  ipv4h = ueNodes.Get(0)->GetObject<Ipv4> ();
  addr = ipv4h->GetAddress (1, 0).GetLocal ();
  std::cout <<"UE  Addr : "<<addr <<" | Interface : 1"<< std::endl;
  addr = ipv4h->GetAddress (2, 0).GetLocal ();
  std::cout <<"UE  Addr : "<<addr <<" | Interface : 2"<< std::endl;
  std::cout <<"=================================="<<std::endl;
  lteHelper->EnableLogComponents ();
  Simulator::Stop (Seconds (180.));
  Simulator::Run ();
  Simulator::Destroy ();

  std::cout <<"Simulation finished!"<<std::endl;

  return 0;
}



