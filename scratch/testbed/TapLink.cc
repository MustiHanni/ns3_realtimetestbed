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
#include "TapLink.h"

namespace ns3 {

Testbed_Link::~Testbed_Link(){
	NS_LOG_FUNCTION (this);
}
Testbed_Link::Testbed_Link(){

	NS_LOG_FUNCTION (this);
	this->m_tapName="";
	this->m_tapAddress=Ipv4Address("0.0.0.0");
	this->m_linkNode=NULL;
	this->getwayAddrToLteNet=Ipv4Address("0.0.0.0");
	this->getwayIfaceToLteNet=-1;

	this->m_TapLeg =  CreateObject<Node>();
}
void Testbed_Link::create_link( std::string tapname, Ipv4Address tapAddr, Ipv4Mask tapMask ){

	NS_LOG_FUNCTION (this);
	NS_ABORT_MSG_IF( tapAddr == Ipv4Address("0.0.0.0") , "Please Set a valid tap-IP address");

	InternetStackHelper internet;
	this->m_tapName = tapname;
	this->m_tapAddress = tapAddr;
	this->m_tapMask = tapMask;
	this->m_linkNode = CreateObject<Node>();

    if ( getLinkNode()->GetObject<Ipv4> () == 0){
    	internet.Install( getLinkNode() );
    }

	CsmaHelper csmah;
	csmah.SetChannelAttribute ("DataRate", DataRateValue (DataRate("5000Gb/s")));
	csmah.SetDeviceAttribute ("Mtu", UintegerValue (1500));
	csmah.SetChannelAttribute ("Delay", TimeValue (Seconds (0.0)));
	NodeContainer csmaNodes;
	csmaNodes.Add( getLinkNode() );
	csmaNodes.Add( getLinkTap() );
	NetDeviceContainer csmaDevice = csmah.Install (csmaNodes);

	Ipv4AddressHelper ipv4h;
	ipv4h.SetBase( getLinkTapAddress().CombineMask( getLinkTapMask() ) , getLinkTapMask() );
	Ipv4InterfaceContainer csmaIpIfaces = ipv4h.Assign(csmaDevice.Get(0));

	TapBridgeHelper tapBridge;
	tapBridge.SetAttribute ("Mode", StringValue ("UseBridge"));
	tapBridge.SetAttribute ("DeviceName", StringValue (m_tapName));
	tapBridge.Install ( getLinkTap() , csmaDevice.Get (1) );
}
}


