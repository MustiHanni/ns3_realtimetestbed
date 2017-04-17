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

int
main (int argc, char *argv[]){

	Testbed mytestbedclass;

	mytestbedclass.commandLineConfiguration(argc,argv);
	mytestbedclass.readFileConfiguration();

	Simulator::Stop(Seconds(mytestbedclass.m_simTime));
	Simulator::Run();
	Simulator::Destroy();

	return 0;
}
