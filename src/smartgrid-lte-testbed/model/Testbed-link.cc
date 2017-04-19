/*
 * Testbed_Link-link.cc
 *
 *  Created on: Mar 26, 2017
 *      Author: musti
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
#include "Testbed-link.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Smartgrid-link");

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

    if ((this->m_linkNode)->GetObject<Ipv4> () == 0){
    	internet.Install(this->m_linkNode);
    }

	CsmaHelper csmah;
	csmah.SetChannelAttribute ("DataRate", DataRateValue (DataRate("5000Gb/s")));
	csmah.SetDeviceAttribute ("Mtu", UintegerValue (1500));
	csmah.SetChannelAttribute ("Delay", TimeValue (Seconds (0.0)));
	NodeContainer csmaNodes;
	csmaNodes.Add(this->m_linkNode);
	csmaNodes.Add(this->m_TapLeg);
	NetDeviceContainer csmaDevice = csmah.Install (csmaNodes);

	Ipv4AddressHelper ipv4h;
	ipv4h.SetBase( this->m_tapAddress.CombineMask( this->m_tapMask ) , this->m_tapMask);
	Ipv4InterfaceContainer csmaIpIfaces = ipv4h.Assign(csmaDevice.Get(0));

	TapBridgeHelper tapBridge;
	tapBridge.SetAttribute ("Mode", StringValue ("UseBridge"));
	tapBridge.SetAttribute ("DeviceName", StringValue (m_tapName));
	tapBridge.Install ( this->m_TapLeg , csmaDevice.Get (1) );
}
}


