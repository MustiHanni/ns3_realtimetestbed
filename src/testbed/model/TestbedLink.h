/*
 * TestbedLink.h
 *
 *  Created on: Apr 13, 2017
 *      Author: ns3
 */

#ifndef SRC_TESTBED_MODEL_TESTBEDLINK_H_
#define SRC_TESTBED_MODEL_TESTBEDLINK_H_
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

class Testbed_Link {
public:
//	Testbed_Link();
//	virtual ~Testbed_Link();

	Testbed_Link();
	~Testbed_Link();

};

} /* namespace ns3 */

#endif /* SRC_TESTBED_MODEL_TESTBEDLINK_H_ */
