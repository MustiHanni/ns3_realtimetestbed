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
#include <string>
#include "ns3/csma-module.h"
#include "ns3/tap-bridge-module.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/ipv4-list-routing.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/lte-module.h"
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
#include "ns3/config-store-module.h"
#include "ns3/config-store.h"
#include "ns3/gtk-config-store.h"
#include "ns3/smartgrid-lte-testbed.h"
#include "ns3/Testbed-link.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LenaTest");
void setTraceConfig();
static void VNetD_MacTx(std::string path, Ptr<const Packet> packet);
static void VNetD_MacRx(std::string path, Ptr<const Packet> packet);
static void VNetD_MacPromiscRx(std::string path, Ptr<const Packet> packet);
static void myTraceCallback (std::string path, Ptr<const Packet> packet, Ptr<Ipv4> ipv4h,  uint32_t interface);
static void csmaTraceTx(std::string path, Ptr< const Packet > packet);
static void csmaTraceRx(std::string path, Ptr< const Packet > packet);

static void p2pTraceTx(std::string path, Ptr< const Packet > packet);
static void p2pTraceRx(std::string path, Ptr< const Packet > packet);

static void UEtraceRRCrandomAccessSuccessful(std::string path, uint64_t imsi, uint16_t cellid, uint16_t rnti);
static void UEtraceRRCconnectionEstablished(std::string path, uint64_t imsi, uint16_t cellid, uint16_t rnti);
static void UEtraceRRCconnectionReconfiguration(std::string path, uint64_t imsi, uint16_t cellid, uint16_t rnti);

static void eNBtraceDLtx(std::string path, Ptr<const PacketBurst> packets);
static void eNBtraceULrx(std::string path, Ptr<const Packet> packet);
static void UEtraceDLrx(std::string path, Ptr<const Packet> packet);
static void UEtraceULtx(std::string path, Ptr<const PacketBurst> packets);

int
main (int argc, char *argv[])
{
	Testbed mytestbedclass;

	mytestbedclass.commandLineConfiguration(argc,argv);
	mytestbedclass.readFileConfiguration();

	setTraceConfig();

  Simulator::Stop(Seconds(mytestbedclass.m_simTime));
  Simulator::Run();
  Simulator::Destroy();
  return 0;

}
void setTraceConfig()
{

	if(0){

	Config::Connect ("/NodeList/*/DeviceList/*/$ns3::VirtualNetDevice/MacTx", MakeCallback (&VNetD_MacTx));
	Config::Connect ("/NodeList/*/DeviceList/*/$ns3::VirtualNetDevice/MacRx", MakeCallback (&VNetD_MacRx));
	Config::Connect ("/NodeList/*/DeviceList/*/$ns3::VirtualNetDevice/MacPromiscRx", MakeCallback (&VNetD_MacPromiscRx));
    Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteEnbNetDevice/LteEnbPhy/DlSpectrumPhy/TxEnd",  MakeCallback (&eNBtraceDLtx));
    Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteEnbNetDevice/LteEnbPhy/UlSpectrumPhy/RxEndOk",  MakeCallback (&eNBtraceULrx));
    Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/LteUePhy/DlSpectrumPhy/RxEndOk",  MakeCallback (&UEtraceDLrx));
    Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteUeNetDevice/LteUePhy/UlSpectrumPhy/TxEnd",  MakeCallback (&UEtraceULtx));
	    Config::Connect ("/NodeList/1/$ns3::Ipv4L3Protocol/Tx", MakeCallback (&myTraceCallback));
		Config::Connect ("/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/PhyTxEnd",MakeCallback (&p2pTraceTx));
		Config::Connect ("/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/PhyRxEnd",MakeCallback (&p2pTraceRx));

    //working
	Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteNetDevice/$ns3::LteUeNetDevice/LteUeRrc/RandomAccessSuccessful",  MakeCallback (&UEtraceRRCrandomAccessSuccessful));
    Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteNetDevice/$ns3::LteUeNetDevice/LteUeRrc/ConnectionEstablished",  MakeCallback (&UEtraceRRCconnectionEstablished));
    Config::Connect ("/NodeList/*/DeviceList/*/$ns3::LteNetDevice/$ns3::LteUeNetDevice/LteUeRrc/ConnectionReconfiguration",  MakeCallback (&UEtraceRRCconnectionReconfiguration));

    Config::Connect ("/NodeList/*/DeviceList/*/$ns3::CsmaNetDevice/PhyTxEnd",MakeCallback (&csmaTraceTx));
    Config::Connect ("/NodeList/*/DeviceList/*/$ns3::CsmaNetDevice/PhyRxEnd",MakeCallback (&csmaTraceRx));
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
