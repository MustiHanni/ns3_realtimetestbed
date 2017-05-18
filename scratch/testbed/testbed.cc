/*
 * testbed.cc
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
#include <iostream>
#include <iomanip>
#include <string>
#include <ctime>
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
#include "Lte-Testbed.h"
#include "TapLink.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LenaTest");

int
main (int argc, char *argv[]){

	time_t t= time(0);
	struct tm * timeNow = localtime(&t);
	uint32_t new_seed = (uint32_t)(timeNow->tm_year + timeNow->tm_mon
			+ timeNow->tm_mday + timeNow->tm_hour + timeNow->tm_min + timeNow->tm_sec);
	RngSeedManager::SetSeed(new_seed);

	Testbed test;

	test.commandLineConfiguration(argc,argv);
	test.readFileConfiguration();

	test.startSimulation();

	return 0;
}
