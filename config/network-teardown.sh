#!/bin/sh
sudo lxc-destroy -n lxc-0
sudo lxc-destroy -n lxc-1
sudo ifconfig br-0 down
sudo ifconfig br-1 down
sudo brctl delif br-0 tap-0
sudo brctl delif br-1 tap-1
sudo brctl delbr br-0
sudo brctl delbr br-1
sudo ifconfig tap-0 down
sudo ifconfig tap-1 down
sudo tunctl -d tap-0
sudo tunctl -d tap-1
