#!/bin/sh

ESSID='click-castor'
CHANNEL=6
DEV='wlan0'
CLICK_PATH=/root/click-castor

killall -q click

iwconfig $DEV mode ad-hoc
iwconfig $DEV essid $ESSID
iwconfig $DEV channel $CHANNEL

MYADDR=`hostname -i | awk -F '.' '{printf("10.20.30.%d", $4)}'`
ifconfig $DEV $MYADDR/16 up

# disable direct forwarding on wlan0
ip route flush dev $DEV

# start Click
cd $CLICK_PATH
bin/click conf/castor/castor_multicast_via_unicast_routing.click
