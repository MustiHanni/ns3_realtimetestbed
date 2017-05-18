#!/bin/sh
route add default gw 1.0.0.1 eth0
ip route add 1.0.1.0/24 via 1.0.0.1 src 1.0.0.2
