#!/bin/sh
sudo lxc-destroy -n lxc-0
sudo lxc-destroy -n lxc-1
sudo lxc-destroy -n lxc-2
sudo lxc-destroy -n lxc-3
sudo lxc-destroy -n lxc-4
sudo lxc-destroy -n lxc-5
sudo lxc-destroy -n lxc-6
sudo ifconfig br-0 down
sudo ifconfig br-1 down
sudo ifconfig br-2 down
sudo ifconfig br-3 down
sudo ifconfig br-4 down
sudo ifconfig br-5 down
sudo ifconfig br-6 down
sudo brctl delif br-0 tap-0
sudo brctl delif br-1 tap-1
sudo brctl delif br-2 tap-2
sudo brctl delif br-3 tap-3
sudo brctl delif br-4 tap-4
sudo brctl delif br-5 tap-5
sudo brctl delif br-6 tap-6
sudo brctl delbr br-0
sudo brctl delbr br-1
sudo brctl delbr br-2
sudo brctl delbr br-3
sudo brctl delbr br-4
sudo brctl delbr br-5
sudo brctl delbr br-6
sudo ifconfig tap-0 down
sudo ifconfig tap-1 down
sudo ifconfig tap-2 down
sudo ifconfig tap-3 down
sudo ifconfig tap-4 down
sudo ifconfig tap-5 down
sudo ifconfig tap-6 down
sudo tunctl -d tap-0
sudo tunctl -d tap-1
sudo tunctl -d tap-2
sudo tunctl -d tap-3
sudo tunctl -d tap-4
sudo tunctl -d tap-5
sudo tunctl -d tap-6
