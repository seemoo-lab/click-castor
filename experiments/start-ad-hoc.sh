#!/bin/sh

ESSID='click-castor'
# 5 GHz band
#CHANNEL=149
# 2.4 GHz band
CHANNEL=14
DEV='wlan0'

killall -q click

ifconfig ${DEV} down
iwconfig ${DEV} mode ad-hoc
iwconfig ${DEV} essid ${ESSID}
iwconfig ${DEV} channel ${CHANNEL}

MYADDR=`hostname -i | awk -F '.' '{printf("10.0.0.%d", $4)}'`
ifconfig ${DEV} ${MYADDR}/16 up

iwconfig ${DEV} rate 12M fixed
#iwconfig ${DEV} rate auto
