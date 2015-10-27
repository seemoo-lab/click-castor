#!/bin/bash

HOST_CSV=nodes.hosts
# First parameter incidates field in CSV
FIELD=''
# Second parameter indicates node ID (if left out, list all) 
NODE=''

while getopts 'i:t:' flag; do
  case "${flag}" in
    i) NODE=${OPTARG} ;;
    t) FIELD=${OPTARG} ;;
    h) HOST_CSV=${OPTARG} ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
    *) error "Unexpected option ${flag}"
  esac
done

if [[ ${FIELD} == '' ]]; then
	echo "ERROR: No field type specified." >&2
	exit 1
fi

# Find out all column fields
IFS=, read fields < ${HOST_CSV} 

fieldsreader="IFS=, read ${fields//,/ }"

# Remove title line and pipe to processor
tail -n +2 ${HOST_CSV} | while eval $fieldsreader; do
	if [[ ${NODE:-${ID}} == ${ID} ]]
	then
		echo ${!FIELD}
	fi
done
