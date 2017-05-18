#!/bin/sh
echo "sh /home/ns3/ns-allinone-3.26/ns-3.26/config/routing-0.sh ; node /home/ns3/ns-allinone-3.26/ns-3.26/server.js --RunTime 5 ; sleep 1" | sudo /usr/bin/lxc-execute -n lxc-0 -f /home/ns3/ns-allinone-3.26/ns-3.26/config/lxc-0.conf /bin/bash &
echo "sh /home/ns3/ns-allinone-3.26/ns-3.26/config/routing-1.sh ; node /home/ns3/ns-allinone-3.26/ns-3.26/client.js --RunTime 5 ; sleep 1" | sudo /usr/bin/lxc-execute -n lxc-1 -f /home/ns3/ns-allinone-3.26/ns-3.26/config/lxc-1.conf /bin/bash &
