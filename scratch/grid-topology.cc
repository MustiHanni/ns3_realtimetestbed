/*
 * grid-topology.cc
 *
 *  Created on: Apr 16, 2017
 *      Author: ns3
 */


/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include <math.h>

using namespace ns3;

int main (int argc, char *argv[]) {
	CommandLine cmd;
    cmd.Parse (argc, argv);

    NodeContainer nodes;



    MobilityHelper mobility;
    double nrOfeNBs = 8;
	double enbDist = 100;
	// create an array of empty nodes for testing purposes
    nodes.Create (nrOfeNBs);
    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
    		"MinX", DoubleValue ( 0 ),
            "MinY", DoubleValue ( 0 ),
            "DeltaX", DoubleValue (enbDist),
            "DeltaY", DoubleValue (enbDist),
			"GridWidth", UintegerValue ( floor(sqrt(nrOfeNBs)) ),
			"LayoutType", StringValue ("RowFirst"));

/*    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
    		"MinX", DoubleValue (-100.0),
            "MinY", DoubleValue (-100.0),
            "DeltaX", DoubleValue (100.0),
            "DeltaY", DoubleValue (100.0),
			"GridWidth", UintegerValue (3),
			"LayoutType", StringValue ("RowFirst"));*/

    // position will never change.
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

    // finalize the setup by attaching to each object in the input array a position and initializing
    mobility.Install (nodes);

    // iterate our nodes and print their position.
    for (NodeContainer::Iterator j = nodes.Begin (); j != nodes.End (); ++j){
    	Ptr<Node> object = *j;
        Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
        NS_ASSERT (position != 0);
        Vector pos = position->GetPosition ();
        std::cout << "x=" << pos.x << ", y=" << pos.y << ", z=" << pos.z << std::endl;
    }

    Simulator::Destroy ();
    return 0;
}

