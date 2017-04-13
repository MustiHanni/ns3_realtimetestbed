/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef SMARTGRID_LTE_TESTBED_H
#define SMARTGRID_LTE_TESTBED_H
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
#include "Testbed-link.h"

namespace ns3 {


class Testbed:public SimpleRefCount<Testbed>{

public:
	Testbed();
	~Testbed();
	void commandLineConfiguration(int argc, char *argv[]);
	void readFileConfiguration();

	void installTestbedLink(Ptr<Testbed_Link> link);
	void installTestbedLink(Ptr<Testbed_Link> link,Ipv4Address p2pNetworkAddress,Ipv4Mask p2pNetworkMask);

	void connectLinks(Ptr<Testbed_Link> firstLink,Ptr<Testbed_Link>secLink);

	double m_simTime ;
	uint8_t m_numOfUe;
	Ptr<LteHelper> m_lteHelper;
	Ptr<PointToPointEpcHelper>  m_epcHelper;
	Ptr<Node> m_pgw;

	std::list<Ptr<Testbed_Link>> m_rh_Links;
	std::list<Ptr<Testbed_Link>> m_ue_Links;

	std::list<Ptr<Node>> m_eNbs;

	std::string m_configFileName;
protected:

private:
};
}

#endif /* SMARTGRID_LTE_TESTBED_H */

