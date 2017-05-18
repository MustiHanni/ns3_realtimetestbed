/*
 * Lte-LteSimu.cc
 *
 *  Created on: Apr 20, 2017
 *      Author: asa
 */


#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <pwd.h>
#include <math.h>
#include <time.h>
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
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "LteSimu.h"

#include "LteSimuLink.h"

namespace ns3 {

#define PI 3.14159265

LteSimu::LteSimu()
{
	NS_LOG_FUNCTION (this);
	m_EnableP2pPcapTraces=true;
	m_EnableAllLteTraces=true;
	m_EnableUeEpcBearer=false;
	m_simTime = 10;
	m_numOfENB=1;
	m_numOfUE=2;
	m_numOfRH=1;
	m_uePerENB=m_numOfUE/m_numOfENB;
	m_ueDist=40;
	m_enbDist=100;
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	std::string pathTohomeDir(homedir);
	//m_configfilepath= pathTohomeDir.append("/config/LteSimu_Simu_Configuration.txt");
	m_configfilepath= "/home/ns3/ns-allinone-3.26/ns-3.26/config/Testbed_Simu_Configuration.txt";

    dlPort = 1234;
    ulPort = 2000;
}

LteSimu::~LteSimu()
{
	  NS_LOG_FUNCTION (this);
}

void LteSimu::commandLineConfiguration(int argc, char *argv[])
{
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO ("Check for user configuration input");
	// Command line arguments
	CommandLine cmd;

	// We are interacting with the outside, world. we have to use the real-time simulator and calculate checksums.
	GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
	GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
	Config::SetDefault("ns3::RealtimeSimulatorImpl::SynchronizationMode",EnumValue(RealtimeSimulatorImpl::SYNC_BEST_EFFORT));

	cmd.AddValue("m_simTime", "Total duration of the simulation [s])", m_simTime);
	cmd.AddValue("m_configfilepath", "Name of Configuration file to set LteSimu topologie", m_configfilepath);
	cmd.Parse(argc, argv);

	m_lteHelper = CreateObject<LteHelper> ();
	m_epcHelper = CreateObject<PointToPointEpcHelper> ();

	m_lteHelper->SetEpcHelper (m_epcHelper);

	ConfigStore inputConfig;
	inputConfig.ConfigureDefaults();

	m_pgw = m_epcHelper->GetPgwNode ();
	// parse again so you can override default values from the command line
	cmd.Parse(argc, argv);
}

void LteSimu::readFileConfiguration(){

	int cellID;
	int positionInCell;
	std::string lineContents;
	std::string infotype;
	std::string p2pAddr;
	std::string p2pMask;
	std::string ghAddr;
	std::string ghMask;

	std::ifstream ConfigFile_(m_configfilepath);
	if(ConfigFile_.is_open()){

		while( std::getline(ConfigFile_,lineContents) ){

			std::stringstream linkInfoStream(lineContents);

			linkInfoStream >> infotype;

			Ptr<LteSimu_Link> link = Create<LteSimu_Link> ();

			if(infotype=="grid_info"){

				linkInfoStream >> this->m_simTime;
				linkInfoStream >> this->m_numOfUE;
				linkInfoStream >> this->m_numOfENB;
				linkInfoStream >> this->m_uePerENB;
				linkInfoStream >> this->m_enbDist;
				linkInfoStream >> this->m_ueDist;

				if(this->m_numOfUE == 0 || this->m_numOfUE ==0){
					ConfigFile_.close();
				}

				m_eNbsNodeContainer.Create (m_numOfENB);

				MobilityHelper enbMmobility;
				enbMmobility.SetPositionAllocator ("ns3::GridPositionAllocator",
			    		"MinX", DoubleValue ( 0.0 ),
			            "MinY", DoubleValue ( 0.0 ),
			            "DeltaX", DoubleValue (this->m_enbDist),
			            "DeltaY", DoubleValue (this->m_enbDist),
						"GridWidth", UintegerValue ( floor(sqrt(this->m_numOfENB)) ),
						"LayoutType", StringValue ("RowFirst"));

			    // position will never change.
				enbMmobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

			    // finalize the setup by attaching to each object in the input array a position and initializing
				enbMmobility.Install (m_eNbsNodeContainer);

			    this->m_eNbsDeviceContainer = this->m_lteHelper->InstallEnbDevice (m_eNbsNodeContainer);

				continue;
			}else if(infotype=="to_pgw"){

				linkInfoStream >> p2pAddr;
				linkInfoStream >> p2pMask;
				linkInfoStream >> ghAddr;
				linkInfoStream >> ghMask;

				link->create_link(Ipv4Address(ghAddr.c_str ()),Ipv4Mask(ghMask.c_str ()));
				this->installLteSimuLink(link,Ipv4Address(p2pAddr.c_str()),Ipv4Mask(p2pMask.c_str ()));

			}else if(infotype=="to_ue"){

				linkInfoStream >> cellID;
				linkInfoStream >> positionInCell;
				linkInfoStream >> ghAddr;
				linkInfoStream >> ghMask;

				link->create_link(Ipv4Address(ghAddr.c_str ()),Ipv4Mask(ghMask.c_str ()));
				this->installLteSimuLink(link,cellID,positionInCell);

			}else{				//std::cout<<" Configfile  Test : "<< infotype <<std::endl;
				continue;
			}
			if(!m_LteSimuLinks.empty()){
				for(std::list<Ptr<LteSimu_Link>>::const_iterator iterator= m_LteSimuLinks.begin(),end= m_LteSimuLinks.end();iterator != end; ++iterator){
					this->connectLinks((*iterator),link);
				}
			}
			m_LteSimuLinks.push_back(link);
		}
		ConfigFile_.close();
	}else{
		NS_ABORT_MSG("Not able to open config file");
	}
}

void LteSimu::installLteSimuLink(Ptr<LteSimu_Link> link, int cellID, int position)
{

	this->installMobilityModel(link, cellID, position);

    NetDeviceContainer ueDev = this->m_lteHelper->InstallUeDevice(link->m_linkNode);

    Ipv4InterfaceContainer ueIpIface = this->m_epcHelper->AssignUeIpv4Address ( ueDev );

    //ueIpIface = ipv4h.Assign(p2pDevices);
    link->getwayAddrToLteNet=this->m_epcHelper->GetUeDefaultGatewayAddress();
    link->getwayIfaceToLteNet=link->m_linkNode->GetObject<Ipv4>()->GetInterfaceForAddress( ueIpIface.GetAddress(0,0) );

    // Assign IP address to UEs
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
	Ptr<Ipv4StaticRouting> StaticRouting;
    StaticRouting = ipv4RoutingHelper.GetStaticRouting (link->m_linkNode->GetObject<Ipv4> ());
    StaticRouting->SetDefaultRoute (this->m_epcHelper->GetUeDefaultGatewayAddress (), 1);

	StaticRouting = ipv4RoutingHelper.GetStaticRouting (m_pgw->GetObject<Ipv4> ());
	StaticRouting->AddNetworkRouteTo(
			link->getLinkghAddress().CombineMask(link->getLinkghMask()),
			link->getLinkghMask(),
			ueIpIface.GetAddress(0,0),
			m_pgw->GetObject<Ipv4>()->GetInterfaceForAddress(this->m_epcHelper->GetUeDefaultGatewayAddress()));

	//make EpcPgwApp forward packet to network beyond UE
	mapGhAddrToUE(link->getLinkghAddress(),ueIpIface.GetAddress(0,0));
	//this->m_epcHelper->m_sgwPgwApp->m_ueAddrByTapAddrMap[link->m_tapAddress] = ueIpIface.GetAddress(0,0);
	//this->m_epcHelper->AddTapAddrToMap(link->getLinkTapAddress(),ueIpIface.GetAddress(0,0));

    // Attach one UE per eNodeB
    this->m_lteHelper->Attach (ueDev, m_eNbsDeviceContainer.Get(cellID));

    if(this->m_EnableUeEpcBearer){
    	enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    	EpsBearer bearer(q);
    	this->m_lteHelper->ActivateDataRadioBearer(ueDev,bearer);
    }

    Ipv4Address remoteHostAddr = m_LteSimuLinks.front()->m_ghLeg->GetObject<Ipv4>()->GetAddress (1, 0).GetLocal();

//     ++ulPort;
//    ApplicationContainer clientApps;
//    ApplicationContainer serverApps;

    if(m_LteSimuLinks.empty()){
    	std::cout<<"link list is empty"<<std::endl;
    }else{
/*
    	PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
    	PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
    	serverApps.Add (dlPacketSinkHelper.Install (link->m_ghLeg));
    	serverApps.Add (ulPacketSinkHelper.Install (m_LteSimuLinks.front()->m_ghLeg));

		UdpClientHelper dlClient (ueIpIface.GetAddress (0), dlPort);
		dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(100)));
		dlClient.SetAttribute ("MaxPackets", UintegerValue(1000000));

		UdpClientHelper ulClient (remoteHostAddr, ulPort);
		ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(100)));
		ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));

		clientApps.Add (dlClient.Install (m_LteSimuLinks.front()->m_ghLeg));
		clientApps.Add (ulClient.Install (link->m_ghLeg));*/
//*************************************************//

		BulkSendHelper source ("ns3::TcpSocketFactory",InetSocketAddress (remoteHostAddr, Port));
		// Set the amount of data to send in bytes.  Zero is unlimited.
		source.SetAttribute ("MaxBytes", UintegerValue (0));
		clientApps = source.Install (link->m_ghLeg);

		// Create a PacketSinkApplication and install it on node 1
		PacketSinkHelper sink ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), Port));
		serverApps = sink.Install(m_LteSimuLinks.front()->m_ghLeg);
    }
}

void LteSimu::installLteSimuLink(Ptr<LteSimu_Link> link,Ipv4Address p2pNetworkAddress,Ipv4Mask p2pNetworkMask){

    NS_LOG_FUNCTION (this);
	Ipv4StaticRoutingHelper ipv4RoutingHelper;
	Ptr<Ipv4StaticRouting> StaticRouting;
    PointToPointHelper p2ph;
    NetDeviceContainer p2pDevices;
	Ipv4AddressHelper ipv4h;
	Ipv4InterfaceContainer p2pIpInterface;

    if ( link->m_linkNode->GetObject<Ipv4>() == 0){
    	NS_FATAL_ERROR ("LteSimu_Link::SmartGrid_Link_install(Ptr<Node> pgw): Aggregating "
                        " link node is without InternetStack");
    	return;
    }

    p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("5000Gb/s")));
    p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
    p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.00000010)));
    p2pDevices = p2ph.Install (this->m_pgw,link->m_linkNode);

    ipv4h.SetBase (p2pNetworkAddress,p2pNetworkMask);
    p2pIpInterface = ipv4h.Assign(p2pDevices);

    link->getwayAddrToLteNet = p2pIpInterface.GetAddress(0,0);
    link->getwayIfaceToLteNet = link->m_linkNode->GetObject<Ipv4>()->GetInterfaceForAddress(p2pIpInterface.GetAddress(1,0));

    StaticRouting=ipv4RoutingHelper.GetStaticRouting(this->m_pgw->GetObject<Ipv4> ());
    StaticRouting->AddNetworkRouteTo(
    		link->getLinkghAddress().CombineMask(link->getLinkghMask()),
			link->getLinkghMask(),
			p2pIpInterface.GetAddress(1,0),
			this->m_pgw->GetObject<Ipv4>()->GetInterfaceForAddress(p2pIpInterface.GetAddress(0,0)));

    if(this->m_EnableP2pPcapTraces){
    	p2ph.EnablePcapAll("LteSimu");
    }
}

void LteSimu::connectLinks(Ptr<LteSimu_Link> firstLink,Ptr<LteSimu_Link> secondLink){

	  Ipv4StaticRoutingHelper ipv4RoutingHelper;
	  Ptr<Ipv4StaticRouting> StaticRouting;

	  StaticRouting = ipv4RoutingHelper.GetStaticRouting (firstLink->m_linkNode->GetObject<Ipv4> ());
	  StaticRouting->AddNetworkRouteTo(
			  secondLink->getLinkghAddress().CombineMask(secondLink->getLinkghMask()),
			  secondLink->getLinkghMask(),
			  firstLink->getwayAddrToLteNet,
			  firstLink->getwayIfaceToLteNet);

	  StaticRouting = ipv4RoutingHelper.GetStaticRouting (secondLink->m_linkNode->GetObject<Ipv4> ());
	  StaticRouting->AddNetworkRouteTo(
			  firstLink->getLinkghAddress().CombineMask(firstLink->getLinkghMask()),
			  firstLink->getLinkghMask(),
			  secondLink->getwayAddrToLteNet,
			  secondLink->getwayIfaceToLteNet);

	  //gh route
	  StaticRouting = ipv4RoutingHelper.GetStaticRouting (firstLink->m_ghLeg->GetObject<Ipv4> ());
	  StaticRouting->AddNetworkRouteTo(
			  secondLink->getLinkghAddress().CombineMask(secondLink->getLinkghMask()),
			  secondLink->getLinkghMask(),
			  firstLink->m_linkNodeAddress,
			  1);//	  firstLink->getwayIfaceToLteNet);

	  //gh route
	  StaticRouting = ipv4RoutingHelper.GetStaticRouting (secondLink->m_ghLeg->GetObject<Ipv4> ());
	  StaticRouting->AddNetworkRouteTo(
			  firstLink->getLinkghAddress().CombineMask(firstLink->getLinkghMask()),
			  firstLink->getLinkghMask(),
			  secondLink->m_linkNodeAddress,
			  1);
}

void LteSimu::installTCPServer(){

}
void LteSimu::installTCPClient(){

}

void LteSimu::installMobilityModel(Ptr<LteSimu_Link> link, int cellID, int cellPosition){

	Ptr<Node> enb = this->m_eNbsNodeContainer.Get(cellID);

    Vector pos = enb->GetObject<MobilityModel>()->GetPosition ();

	double angle = (2*PI)/(this->m_uePerENB);
	double xCoorUE = pos.x + round( m_ueDist * cos( angle * cellPosition));
	double yCoorUE = pos.y + round( m_ueDist * sin( angle * cellPosition));

    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc->Add( Vector( xCoorUE, yCoorUE, 0) );

    MobilityHelper ueMmobility;
    ueMmobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    ueMmobility.SetPositionAllocator( positionAlloc );
    ueMmobility.Install(link->getLinkNode());
}
void LteSimu::mapGhAddrToUE(Ipv4Address ghAddress, Ipv4Address ueAddress){
	m_epcHelper->AddTapAddrToMap(ghAddress,ueAddress);
}

void LteSimu::startSimulation(){
	time_t starttime,endtimer;
	double duration;

    serverApps.Start (Seconds (0));
    clientApps.Start (Seconds (0));
    serverApps.Stop (Minutes(this->m_simTime));
    clientApps.Stop (Minutes(this->m_simTime));

    if(this->m_EnableAllLteTraces){
    	this->m_lteHelper->EnableTraces();
    }
    //std::cout<<"Simulation started, will take "<<this->m_simTime<<" Minutes"<<std::endl;
	Simulator::Stop(Minutes(this->m_simTime));
	time(&starttime);
	Simulator::Run();
	time(&endtimer);
	Simulator::Destroy();
	duration = difftime(endtimer,starttime);
    std::cout<<"Simulation run duration:  "<< duration <<std::endl;
}
}//ns3 namepspace

