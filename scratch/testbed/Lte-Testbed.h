/*
 * Lte-Testbed.h
 *
 *  Created on: Apr 20, 2017
 *      Author: asa
 */

#ifndef SCRATCH_TESTBED_LTE_TESTBED_H_
#define SCRATCH_TESTBED_LTE_TESTBED_H_
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <string>
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
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include "TapLink.h"

namespace ns3 {


class Testbed:public SimpleRefCount<Testbed>{

public:
	Testbed();
	~Testbed();
	void commandLineConfiguration(int argc, char *argv[]);
	void readFileConfiguration();

	void installTestbedLink(Ptr<Testbed_Link> link, int cellID, int position);
	void installTestbedLink(Ptr<Testbed_Link> link, Ipv4Address p2pNetworkAddress, Ipv4Mask p2pNetworkMask);

	void installMobilityModel(Ptr<Testbed_Link> link, int cellID, int cellPosition);

	void connectLinks(Ptr<Testbed_Link> firstLink, Ptr<Testbed_Link> secLink);

	void mapTapAddrToUE(Ipv4Address tapAddress, Ipv4Address ueAddress);

	void startSimulation();
	std::string m_configfilepath;
	double m_numOfENB;
	double m_numOfUE;
	double m_numOfRH;
	double m_uePerENB;
	double m_enbDist;
	double m_ueDist;
	double m_simTime ;

	Ptr<LteHelper> m_lteHelper;
	Ptr<PointToPointEpcHelper>  m_epcHelper;

	Ptr<Node> m_pgw;

    NodeContainer m_eNbsNodeContainer;
    NetDeviceContainer m_eNbsDeviceContainer;

	bool m_EnableP2pPcapTraces;
	bool m_EnableAllLteTraces;
	bool m_EnableUeEpcBearer;
protected:

private:
};
}




#endif /* SCRATCH_TESTBED_LTE_TESTBED_H_ */
