#!/bin/sh
sudo brctl addbr br-0
sudo brctl addbr br-1
sudo tunctl -t tap-0
sudo tunctl -t tap-1
sudo ifconfig tap-0 0.0.0.0 promisc up
sudo ifconfig tap-1 0.0.0.0 promisc up
sudo brctl addif br-0 tap-0
sudo brctl addif br-1 tap-1
sudo ifconfig br-0 up
sudo ifconfig br-1 up
sudo brctl show
sudo modprobe br_netfilter
sudo lxc-create -n lxc-0 -f lxc-0.conf
sudo lxc-create -n lxc-1 -f lxc-1.conf
