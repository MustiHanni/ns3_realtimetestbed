/*
 * Lte-Testbed.cc
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
#include "Lte-Testbed.h"
#include "TapLink.h"

namespace ns3 {

#define PI 3.14159265

Testbed::Testbed()
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
	//m_configfilepath= pathTohomeDir.append("/config/Testbed_Simu_Configuration.txt");
	m_configfilepath= "/home/ns3/ns-allinone-3.26/ns-3.26/config/Testbed_Simu_Configuration.txt";
}

Testbed::~Testbed()
{
	  NS_LOG_FUNCTION (this);
}

void Testbed::commandLineConfiguration(int argc, char *argv[])
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
	cmd.AddValue("m_configfilepath", "Name of Configuration file to set Testbed topologie", m_configfilepath);
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

void Testbed::readFileConfiguration(){

	int cellID;
	int positionInCell;
	std::string lineContents;
	std::string infotype;
	std::string p2pAddr;
	std::string p2pMask;
	std::string tapName;
	std::string tapAddr;
	std::string tapMask;

	std::list<Ptr<Testbed_Link>> testbedLinks;
	std::ifstream ConfigFile_(m_configfilepath);
	if(ConfigFile_.is_open()){

		while( std::getline(ConfigFile_,lineContents) ){

			std::stringstream linkInfoStream(lineContents);

			linkInfoStream >> infotype;

			Ptr<Testbed_Link> link = Create<Testbed_Link> ();

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
				linkInfoStream >> tapName;
				linkInfoStream >> tapAddr;
				linkInfoStream >> tapMask;

				link->create_link(tapName,Ipv4Address(tapAddr.c_str ()),Ipv4Mask(tapMask.c_str ()));
				this->installTestbedLink(link,Ipv4Address(p2pAddr.c_str()),Ipv4Mask(p2pMask.c_str ()));

			}else if(infotype=="to_ue"){

				linkInfoStream >> cellID;
				linkInfoStream >> positionInCell;
				linkInfoStream >> tapName;
				linkInfoStream >> tapAddr;
				linkInfoStream >> tapMask;

				link->create_link(tapName,Ipv4Address(tapAddr.c_str ()),Ipv4Mask(tapMask.c_str ()));
				this->installTestbedLink(link,cellID,positionInCell);

			}else{
				//std::cout<<" Configfile  Test : "<< infotype <<std::endl;
				continue;
			}
			if(!testbedLinks.empty()){
				for(std::list<Ptr<Testbed_Link>>::const_iterator iterator= testbedLinks.begin(),end= testbedLinks.end();iterator != end; ++iterator){
					this->connectLinks((*iterator),link);
				}
			}
			testbedLinks.push_back(link);
		}
		ConfigFile_.close();
	}else{
		NS_ABORT_MSG("Not able to open config file");
	}
}

void Testbed::installTestbedLink(Ptr<Testbed_Link> link, int cellID, int position)
{

	this->installMobilityModel(link, cellID, position);

    NetDeviceContainer ueDev = this->m_lteHelper->InstallUeDevice(link->m_linkNode);

    Ipv4InterfaceContainer ueIpIface2 = this->m_epcHelper->AssignUeIpv4Address ( ueDev );

    //ueIpIface2 = ipv4h.Assign(p2pDevices);
    link->getwayAddrToLteNet=this->m_epcHelper->GetUeDefaultGatewayAddress();
    link->getwayIfaceToLteNet=link->m_linkNode->GetObject<Ipv4>()->GetInterfaceForAddress( ueIpIface2.GetAddress(0,0) );

    // Assign IP address to UEs
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
	Ptr<Ipv4StaticRouting> StaticRouting;
    StaticRouting = ipv4RoutingHelper.GetStaticRouting (link->m_linkNode->GetObject<Ipv4> ());
    StaticRouting->SetDefaultRoute (this->m_epcHelper->GetUeDefaultGatewayAddress (), 1);

	StaticRouting = ipv4RoutingHelper.GetStaticRouting (m_pgw->GetObject<Ipv4> ());
	StaticRouting->AddNetworkRouteTo(
			link->getLinkTapAddress().CombineMask(link->getLinkTapMask()),
			link->getLinkTapMask(),
			ueIpIface2.GetAddress(0,0),
			m_pgw->GetObject<Ipv4>()->GetInterfaceForAddress(this->m_epcHelper->GetUeDefaultGatewayAddress()));

	//make EpcPgwApp forward packet to network beyond UE
	mapTapAddrToUE(link->getLinkTapAddress(),ueIpIface2.GetAddress(0,0));
	//this->m_epcHelper->m_sgwPgwApp->m_ueAddrByTapAddrMap[link->m_tapAddress] = ueIpIface2.GetAddress(0,0);
	//this->m_epcHelper->AddTapAddrToMap(link->getLinkTapAddress(),ueIpIface2.GetAddress(0,0));

    // Attach one UE per eNodeB
    this->m_lteHelper->Attach (ueDev, m_eNbsDeviceContainer.Get(cellID));

    if(this->m_EnableUeEpcBearer){
    	enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    	EpsBearer bearer(q);
    	this->m_lteHelper->ActivateDataRadioBearer(ueDev,bearer);
    }
}

void Testbed::installTestbedLink(Ptr<Testbed_Link> link,Ipv4Address p2pNetworkAddress,Ipv4Mask p2pNetworkMask){

    NS_LOG_FUNCTION (this);
	Ipv4StaticRoutingHelper ipv4RoutingHelper;
	Ptr<Ipv4StaticRouting> StaticRouting;
    PointToPointHelper p2ph;
    NetDeviceContainer p2pDevices;
	Ipv4AddressHelper ipv4h;
	Ipv4InterfaceContainer p2pIpInterface;

    if ( link->m_linkNode->GetObject<Ipv4>() == 0){
    	NS_FATAL_ERROR ("Testbed_Link::SmartGrid_Link_install(Ptr<Node> pgw): Aggregating "
                        " link node is without InternetStack");
    	return;
    }

    p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("5000Gb/s")));
    p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
    p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.00000010)));
    p2pDevices = p2ph.Install (this->m_pgw,link->m_linkNode);

    ipv4h.SetBase (p2pNetworkAddress,p2pNetworkMask );
    p2pIpInterface = ipv4h.Assign(p2pDevices);

    link->getwayAddrToLteNet = p2pIpInterface.GetAddress(0,0);
    link->getwayIfaceToLteNet = link->m_linkNode->GetObject<Ipv4>()->GetInterfaceForAddress( p2pIpInterface.GetAddress(1,0) );

    StaticRouting = ipv4RoutingHelper.GetStaticRouting (this->m_pgw->GetObject<Ipv4> ());
    StaticRouting->AddNetworkRouteTo(
    		link->getLinkTapAddress().CombineMask(link->getLinkTapMask()),
			link->getLinkTapMask(),
			p2pIpInterface.GetAddress(1,0),
			this->m_pgw->GetObject<Ipv4>()->GetInterfaceForAddress( p2pIpInterface.GetAddress(0,0) ) );
    if(this->m_EnableP2pPcapTraces){
    	p2ph.EnablePcapAll("Testbed");
    }
}

void Testbed::connectLinks(Ptr<Testbed_Link> firstLink,Ptr<Testbed_Link> secondLink){

	  Ipv4StaticRoutingHelper ipv4RoutingHelper;
	  Ptr<Ipv4StaticRouting> StaticRouting;
	  StaticRouting = ipv4RoutingHelper.GetStaticRouting (firstLink->m_linkNode->GetObject<Ipv4> ());
	  StaticRouting->AddNetworkRouteTo(
			  secondLink->getLinkTapAddress().CombineMask(secondLink->getLinkTapMask()),
			  secondLink->getLinkTapMask(),
			  firstLink->getwayAddrToLteNet,
			  firstLink->getwayIfaceToLteNet);

	  StaticRouting = ipv4RoutingHelper.GetStaticRouting (secondLink->m_linkNode->GetObject<Ipv4> ());
	  StaticRouting->AddNetworkRouteTo(
			  firstLink->getLinkTapAddress().CombineMask(firstLink->getLinkTapMask()),
			  firstLink->getLinkTapMask(),
			  secondLink->getwayAddrToLteNet,
			  secondLink->getwayIfaceToLteNet);
}

void Testbed::installMobilityModel(Ptr<Testbed_Link> link, int cellID, int cellPosition){

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
void Testbed::mapTapAddrToUE(Ipv4Address tapAddress, Ipv4Address ueAddress){
	m_epcHelper->AddTapAddrToMap(tapAddress,ueAddress);
}

void Testbed::startSimulation(){
	time_t starttime,endtimer;
	double duration;
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

