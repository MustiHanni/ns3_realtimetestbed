#!/bin/sh
sudo brctl addbr br-0
sudo brctl addbr br-1
sudo brctl addbr br-2
sudo brctl addbr br-3
sudo brctl addbr br-4
sudo brctl addbr br-5
sudo brctl addbr br-6
sudo tunctl -t tap-0
sudo tunctl -t tap-1
sudo tunctl -t tap-2
sudo tunctl -t tap-3
sudo tunctl -t tap-4
sudo tunctl -t tap-5
sudo tunctl -t tap-6
sudo ifconfig tap-0 0.0.0.0 promisc up
sudo ifconfig tap-1 0.0.0.0 promisc up
sudo ifconfig tap-2 0.0.0.0 promisc up
sudo ifconfig tap-3 0.0.0.0 promisc up
sudo ifconfig tap-4 0.0.0.0 promisc up
sudo ifconfig tap-5 0.0.0.0 promisc up
sudo ifconfig tap-6 0.0.0.0 promisc up
sudo brctl addif br-0 tap-0
sudo brctl addif br-1 tap-1
sudo brctl addif br-2 tap-2
sudo brctl addif br-3 tap-3
sudo brctl addif br-4 tap-4
sudo brctl addif br-5 tap-5
sudo brctl addif br-6 tap-6
sudo ifconfig br-0 up
sudo ifconfig br-1 up
sudo ifconfig br-2 up
sudo ifconfig br-3 up
sudo ifconfig br-4 up
sudo ifconfig br-5 up
sudo ifconfig br-6 up
sudo brctl show
sudo modprobe br_netfilter
sudo lxc-create -n lxc-0 -f lxc-0.conf
sudo lxc-create -n lxc-1 -f lxc-1.conf
sudo lxc-create -n lxc-2 -f lxc-2.conf
sudo lxc-create -n lxc-3 -f lxc-3.conf
sudo lxc-create -n lxc-4 -f lxc-4.conf
sudo lxc-create -n lxc-5 -f lxc-5.conf
sudo lxc-create -n lxc-6 -f lxc-6.conf
