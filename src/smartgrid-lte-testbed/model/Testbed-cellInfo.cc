/*
 * TestbedcellInfo.cpp
 *
 *  Created on: Apr 9, 2017
 *      Author: musti
 */

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <string>     // std::string, std::to_string
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
#include "Testbed-link.h"
#include "Testbed-cellInfo.h"

#include <iomanip>
#include <string>
#include <math.h>
namespace ns3 {

Testbed_cellInfo::Testbed_cellInfo() {
	m_cellID=0;

	m_nrOfUEs=1;
	m_currentUEnumber=0;
	m_ueDist=10;
	m_positionAngle=(2*PI)/m_nrOfUEs;
	m_currentAngle=m_positionAngle;

	xPosition=0;
	yPosition=0;
}
Testbed_cellInfo::Testbed_cellInfo(int id , double x , double y, int nrUE) {
	m_cellID = id;
	xPosition = x;
	yPosition = y;
	m_currentUEnumber=0;
	m_nrOfUEs = nrUE;
	m_ueDist = 10;
	m_positionAngle = (2*PI)/m_nrOfUEs;
	m_currentAngle = m_positionAngle;
}
void Testbed_cellInfo::getNewUeCoordinate(double &xCoord,double &yCoord){

	this->m_positionAngle=(2*PI)/this->m_nrOfUEs;
	this->m_currentAngle=this->m_currentAngle+this->m_positionAngle;

	xCoord = xPosition + round( this->m_ueDist*cos( this->m_currentAngle));
	yCoord = yPosition + round( this->m_ueDist*sin( this->m_currentAngle));
}
Testbed_cellInfo::~Testbed_cellInfo() {
}

} /* namespace ns3 */
