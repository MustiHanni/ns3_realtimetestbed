/*
 * Testbed_Link-link.h
 *
 *  Created on: Mar 26, 2017
 *      Author: musti
 */

#ifndef SRC_SMARTGRID_LTE_TESTBED_MODEL_Testbed_Link_H_
#define SRC_SMARTGRID_LTE_TESTBED_MODEL_Testbed_Link_H_

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

enum TESTBED_LINKTYPE { UNDEFINED, LINK_TO_PGW, LINK_TO_UE };

class Testbed_Link:public SimpleRefCount<Testbed_Link>{

	public:
	Testbed_Link();
	~Testbed_Link();
	void create_link(TESTBED_LINKTYPE linktype,std::string tapname,Ipv4Address tapAddr,Ipv4Mask tapMask);

	//////////////////// Getter Methods //////////////////////////////////
	Ipv4Mask getLinkTapMask(){return this->m_tapMask;};
	Ptr<Node> getLinkNode(){return this->m_linkNode;};
	std::string getLinkTapName(){return this->m_tapName;};
	Ipv4Address getLinkTapAddress(){return this->m_tapAddress;};
	NodeContainer getLinkTap(){return this->m_TapLeg;};
	TESTBED_LINKTYPE getLinktype(){return this->m_linktype;};
	//////////////////// Setter Methods ///////////////////////////////////
	void setLinktype(TESTBED_LINKTYPE type){this->m_linktype=type;};
	void setLinkNode(Ptr<Node> networkNode){this->m_linkNode=networkNode;};
	void setLinkTapMask(Ipv4Mask mask){this->m_tapMask=mask;};
	void setLinkTapName(std::string tapName){this->m_tapName=tapName;};
	void setLinkTapAddress(Ipv4Address addr){this->m_tapAddress=addr;};
	///////////////////////////////////////////////////////////////////////

	std::string m_tapName;
	Ipv4Address m_tapAddress;
	Ipv4Mask m_tapMask;
	Ptr<Node> m_linkNode;
	NodeContainer m_TapLeg;
	TESTBED_LINKTYPE m_linktype;
	Ipv4Address getwayAddrToLteNet;
	uint32_t getwayIfaceToLteNet;
	};
}

#endif /* SRC_SMARTGRID_LTE_TESTBED_MODEL_Testbed_Link_H_ */
