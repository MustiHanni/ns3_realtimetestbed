/*
 * Lte-Simu.h
 *
 *  Created on: Apr 20, 2017
 *      Author: asa
 */

#ifndef SCRATCH_LTE_SIMU_H_
#define SCRATCH_LTE_SIMU_H_
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <string>

#include "LteSimuLink.h"
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

#include "LteSimuLink.h"

namespace ns3 {


class LteSimu:public SimpleRefCount<LteSimu>{

public:
	LteSimu();
	~LteSimu();
	void commandLineConfiguration(int argc, char *argv[]);
	void readFileConfiguration();

	void installLteSimuLink(Ptr<LteSimu_Link> link, int cellID, int position);
	void installLteSimuLink(Ptr<LteSimu_Link> link, Ipv4Address p2pNetworkAddress, Ipv4Mask p2pNetworkMask);

	void installMobilityModel(Ptr<LteSimu_Link> link, int cellID, int cellPosition);

	void connectLinks(Ptr<LteSimu_Link> firstLink, Ptr<LteSimu_Link> secLink);

	void installTCPServer();
	void installTCPClient();

	void mapGhAddrToUE(Ipv4Address ghAddress, Ipv4Address ueAddress);

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

	std::list<Ptr<LteSimu_Link>> m_LteSimuLinks;

	bool m_EnableP2pPcapTraces;
	bool m_EnableAllLteTraces;
	bool m_EnableUeEpcBearer;


    // Install and start applications on UEs and remote host
	uint16_t Port = 9;
    uint16_t dlPort ;
    uint16_t ulPort ;
    ApplicationContainer clientApps;
    ApplicationContainer serverApps;
protected:

private:
};
}




#endif /* SCRATCH_LTE_SIMU_H_ */
