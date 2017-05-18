/*
 * TapLink.h
 *
 *  Created on: Apr 20, 2017
 *      Author: asa
 */

#ifndef SCRATCH_TESTBED_TAPLINK_H_
#define SCRATCH_TESTBED_TAPLINK_H_


#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
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

class Testbed_Link:public SimpleRefCount<Testbed_Link>{

	public:
	Testbed_Link();
	~Testbed_Link();
	void create_link(std::string tapname,Ipv4Address tapAddr,Ipv4Mask tapMask);

	std::string getLinkTapName(){return this->m_tapName;};

	Ipv4Address getLinkTapAddress(){return this->m_tapAddress;};

	Ipv4Mask getLinkTapMask(){return this->m_tapMask;};

	Ptr<Node> getLinkNode(){return this->m_linkNode;};

	Ptr<Node> getLinkTap(){return this->m_TapLeg;};

	Ipv4Address getGetwayAddrToLteNet(){ return this->getwayAddrToLteNet;}

	uint32_t getGetwayIfaceToLteNet(){ return this->getwayIfaceToLteNet;}

	std::string m_tapName;
	Ipv4Address m_tapAddress;
	Ipv4Mask m_tapMask;
	Ptr<Node> m_linkNode;
	Ptr<Node> m_TapLeg;
	Ipv4Address getwayAddrToLteNet;
	uint32_t getwayIfaceToLteNet;

};
}



#endif /* SCRATCH_TESTBED_TAPLINK_H_ */
