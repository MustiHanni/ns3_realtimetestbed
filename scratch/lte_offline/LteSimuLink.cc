/*
 * TapLink.cc
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
#include <string>     // std::string, std::to_string

#include "LteSimuLink.h"
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

namespace ns3 {

LteSimu_Link::~LteSimu_Link(){
	NS_LOG_FUNCTION (this);
}
LteSimu_Link::LteSimu_Link(){

	NS_LOG_FUNCTION (this);
	this->m_ghAddress=Ipv4Address("0.0.0.0");
	this->m_linkNode=NULL;
	this->getwayAddrToLteNet=Ipv4Address("0.0.0.0");
	this->getwayIfaceToLteNet=-1;
}
void LteSimu_Link::create_link(Ipv4Address ghAddr, Ipv4Mask ghMask ){

	NS_LOG_FUNCTION (this);
	NS_ABORT_MSG_IF( ghAddr == Ipv4Address("0.0.0.0") , "Please Set a valid gh-IP address");

	InternetStackHelper internet;

	this->m_ghAddress = ghAddr;
	this->m_ghMask = ghMask;

	this->m_linkNode = CreateObject<Node>();
	this->m_ghLeg =  CreateObject<Node>();

    if ( getLinkNode()->GetObject<Ipv4> () == 0	){ internet.Install( getLinkNode() ); }
    if ( getLinkgh()->GetObject<Ipv4> () == 0	){ internet.Install( getLinkgh() );	  }

	CsmaHelper csmah;
	csmah.SetChannelAttribute ("DataRate", DataRateValue (DataRate("5000Gb/s")));
	csmah.SetDeviceAttribute ("Mtu", UintegerValue (1500));
	csmah.SetChannelAttribute ("Delay", TimeValue (Seconds (0.0)));

	NodeContainer csmaNodes;

	csmaNodes.Add( getLinkNode() );
	csmaNodes.Add( getLinkgh() );
	NetDeviceContainer csmaDevice = csmah.Install (csmaNodes);

	Ipv4AddressHelper ipv4h;
	ipv4h.SetBase( getLinkghAddress().CombineMask( getLinkghMask() ) , getLinkghMask() );
	Ipv4InterfaceContainer csmaIpIfaces = ipv4h.Assign(csmaDevice);

	m_linkNodeAddress=csmaIpIfaces.GetAddress(0);
	m_linkNodeMask=getLinkghMask();
//	csmah.EnablePcapAll("LteSimu_Link",true);
}
}


