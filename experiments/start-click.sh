#!/bin/sh

CLICK_PATH=/root/click-castor
DEV=wlan0

`dirname $0`/start-ad-hoc.sh

# disable direct forwarding on wlan0
ip route flush dev $DEV

# start Click
$CLICK_PATH/bin/click $CLICK_PATH/conf/castor/castor_multicast_via_unicast_routing.click < /dev/null 1> /dev/null 2>&1 &
