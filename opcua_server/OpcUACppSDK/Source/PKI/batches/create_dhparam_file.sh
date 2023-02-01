#!/bin/bash
# Usage create_dhparam_file.sh 'location_root'

pushd `dirname $0` > /dev/null
BATCH_DIR="$PWD"
popd > /dev/null

if [ "$#" -lt 1 ]; then
	echo "No path has been specified"
	exit 1
fi

LOCATION=$1

DHPARAM_FILE_LOCATION=$LOCATION/private/dhparam.pem

# remove files to create when they already exist
if [ -f "$DHPARAM_FILE_LOCATION" ]; then rm "$DHPARAM_FILE_LOCATION" ; fi

mkdir -p "$LOCATION"
mkdir -p "$LOCATION/private"

# Generate dhparam file (this can take a while)
openssl dhparam -outform PEM -out "$DHPARAM_FILE_LOCATION" 2048

