#!/bin/bash

EXPERIMENT_DIR=/root/click-castor/experiments

NODES=`./nodeinfo.sh -t ID`

REBOOT='no'

while getopts 'r' flag; do
  case "${flag}" in
    r) REBOOT='yes' ;;
    *) error "Unexpected option ${flag}"
  esac
done

echo "--- START NODES (Reboot: ${REBOOT}) ---"

if [[ ${REBOOT} == 'yes' ]]
then
	for n in ${NODES}; do
		echo "(Re)starting node ${n} ..."
		pypwrctrl off `./nodeinfo.sh -i ${n} -t PYPWR` &>/dev/null
	done

	sleep 1

	for n in ${NODES}; do
		pypwrctrl on  `./nodeinfo.sh -i ${n} -t PYPWR` &>/dev/null
	done
fi

echo "Checking if nodes are up ..."
for n in ${NODES}
do
	nodename=`./nodeinfo.sh -i ${n} -t DNS`
	# ':' is no-op command
	#while ! ping -c 1 ${nodename} &>/dev/null; do sleep 0.1; done
	until ssh ${nodename} : &>/dev/null; do
		sleep 1
	done
	echo "Node ${nodename} is up!"
done


echo "--- START CLICK ---"

HOSTS=`mktemp`
./nodeinfo.sh -t DNS > ${HOSTS}

pssh -l root -h ${HOSTS} ${EXPERIMENT_DIR}/start-click.sh

rm ${HOSTS}
