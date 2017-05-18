/*
 * TapLink.h
 *
 *  Created on: Apr 20, 2017
 *      Author: asa
 */

#ifndef SCRATCH_LteSimu_TAPLINK_H_
#define SCRATCH_LteSimu_TAPLINK_H_


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

class LteSimu_Link:public SimpleRefCount<LteSimu_Link>{

	public:
	LteSimu_Link();
	~LteSimu_Link();
	void create_link(Ipv4Address ghAddr,Ipv4Mask ghMask);

	Ipv4Address getLinkghAddress(){return this->m_ghAddress;};

	Ipv4Mask getLinkghMask(){return this->m_ghMask;};

	Ptr<Node> getLinkNode(){return this->m_linkNode;};

	Ptr<Node> getLinkgh(){return this->m_ghLeg;};

	Ipv4Address getGetwayAddrToLteNet(){ return this->getwayAddrToLteNet;}

	uint32_t getGetwayIfaceToLteNet(){ return this->getwayIfaceToLteNet;}

	Ipv4Address m_linkNodeAddress;
	Ipv4Mask m_linkNodeMask;
	Ptr<Node> m_linkNode;
	Ipv4Address m_ghAddress;
	Ipv4Mask m_ghMask;
	Ptr<Node> m_ghLeg;
	Ipv4Address getwayAddrToLteNet;
	uint32_t getwayIfaceToLteNet;

};
}

#endif /* SCRATCH_LteSimu_TAPLINK_H_ */
