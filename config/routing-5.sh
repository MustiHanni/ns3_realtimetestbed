#!/bin/sh
route add default gw 1.0.5.1 eth0
ip route add 1.0.0.0/24 via 1.0.5.1 src 1.0.5.2
ip route add 1.0.1.0/24 via 1.0.5.1 src 1.0.5.2
ip route add 1.0.2.0/24 via 1.0.5.1 src 1.0.5.2
ip route add 1.0.3.0/24 via 1.0.5.1 src 1.0.5.2
ip route add 1.0.4.0/24 via 1.0.5.1 src 1.0.5.2
ip route add 1.0.6.0/24 via 1.0.5.1 src 1.0.5.2
