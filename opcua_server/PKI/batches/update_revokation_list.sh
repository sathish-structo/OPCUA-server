#!/bin/bash
# Usage update_revokation_list.sh 'location_root' 'ca_name'
# where ca_name is the filename without file extention (e.g. myCA)

pushd `dirname $0` > /dev/null
BATCH_DIR="$PWD"
popd > /dev/null

if [ "$#" -lt 1 ]; then
	echo "No path has been specified"
	exit 1
fi
if [ "$#" -lt 2 ]; then
	echo "No CA name has been specified"
	exit 1
fi

LOCATION=$1
CA_NAME=$2

CONFIGURATION=$BATCH_DIR/openssl.cnf
REVOCATION_LIST_LOCATION=$LOCATION/crls/revocation_list_$CA_NAME.crl
CA_CERTIFICATE=$LOCATION/certs/cert_$CA_NAME.pem
CA_PRIVATE_KEY=$LOCATION/private/private_key_$CA_NAME.pem
# These three variables are used by the OpenSSL as environment variables
CA_DATABASE_LOCATION=$LOCATION/database_$CA_NAME.txt
CA_SERIAL_NUMBER_LOCATION=$LOCATION/serial_$CA_NAME.txt
CA_CRL_NUMBER_LOCATION=$LOCATION/crlnumber_$CA_NAME.txt
export CA_DATABASE_LOCATION
export CA_SERIAL_NUMBER_LOCATION
export CA_CRL_NUMBER_LOCATION

# Step 1: Initialize location

mkdir -p "$LOCATION"
mkdir -p "$LOCATION/private"
mkdir -p "$LOCATION/certs"
mkdir -p "$LOCATION/crls"
mkdir -p "$LOCATION/request"

# Step 2: Generate CRL
openssl ca -config "$CONFIGURATION" -gencrl -crldays 30 -out "$REVOCATION_LIST_LOCATION" -passin pass:pass -cert "$CA_CERTIFICATE" -keyfile "$CA_PRIVATE_KEY"

