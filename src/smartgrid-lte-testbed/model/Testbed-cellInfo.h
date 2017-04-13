/*
 * TestbedcellInfo.h
 *
 *  Created on: Apr 9, 2017
 *      Author: musti
 */

#ifndef SRC_SMARTGRID_LTE_TESTBED_MODEL_TESTBED_CELLINFO_H_
#define SRC_SMARTGRID_LTE_TESTBED_MODEL_TESTBED_CELLINFO_H_
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

#define PI 3.14159265

class Testbed_cellInfo:public SimpleRefCount<Testbed_cellInfo> {
public:
	Testbed_cellInfo();
	Testbed_cellInfo(int id, double x, double y, int nrUE);
	void getNewUeCoordinate(double &xCoord,double &yCoord);
	virtual ~Testbed_cellInfo();

	void setCellId(int id){m_cellID=id;}
	void setCellNrUEs(int nrUEs){m_nrOfUEs=nrUEs;}
	void setXposition(int x){xPosition=x;}
	void setYposition(int y){yPosition=y;}
	void setUEdistance(int ueDist){m_ueDist=ueDist;}

	void setPositionAngle( ){m_positionAngle = (2*PI)/m_nrOfUEs;}
	Ptr<Node> m_enb;
	NetDeviceContainer m_enbDev;
	int m_cellID;

	int m_nrOfUEs;
	int m_currentUEnumber;

	double xPosition;
	double yPosition;

	double m_ueDist;
	double m_positionAngle;
	double m_currentAngle;
};

} /* namespace ns3 */

#endif /* SRC_SMARTGRID_LTE_TESTBED_MODEL_TESTBED_CELLINFO_H_ */
