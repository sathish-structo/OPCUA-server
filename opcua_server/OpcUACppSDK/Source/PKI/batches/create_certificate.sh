#!/bin/bash
# Usage create_certificate.sh 'location_root' 'signing_ca_name' 'unique_name' 'subj'
# where signing_ca_name is the filename without file extention (e.g. myCA)
# and subj has the form: "/C=%COUNTRY%/DC=%DomainComponent%/ST=%STATE%/O=%ORGANIZATION%/OU=%ORGANIZATION_UNIT%/CN=%TO%"

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
if [ "$#" -lt 3 ]; then
	echo "No name has been specified"
	exit 1
fi
if [ "$#" -lt 4 ]; then
	echo "No subject has been specified"
	exit 1
fi

LOCATION=$1
SIGNING_CA_NAME=$2
NAME=$3
SUBJ=$4

CONFIGURATION=$BATCH_DIR/openssl.cnf
SIGNING_CA_PEM_CERTIFICATE=$LOCATION/certs/cert_$SIGNING_CA_NAME.pem
SIGNING_CA_PRIVATE_KEY=$LOCATION/private/private_key_$SIGNING_CA_NAME.pem
# These three variables are used by the OpenSSL as environment variables
CA_DATABASE_LOCATION=$LOCATION/database_$SIGNING_CA_NAME.txt
CA_SERIAL_NUMBER_LOCATION=$LOCATION/serial_$SIGNING_CA_NAME.txt
CA_CRL_NUMBER_LOCATION=$LOCATION/crlnumber_$SIGNING_CA_NAME.txt
export CA_DATABASE_LOCATION
export CA_SERIAL_NUMBER_LOCATION
export CA_CRL_NUMBER_LOCATION

PRIVATE_KEY_LOCATION=$LOCATION/private/private_key_$NAME.pem
CERTIFICATE_LOCATION=$LOCATION/certs/cert_$NAME.der
TEMP_CERTIFICATE_LOCATION=$LOCATION/certs/cert_$NAME.pem
REQUEST_LOCATION=$LOCATION/request/req_$NAME.csr
# These three locations are only needed for proxy certificates, and aren't generated here
CERT_DATABASE_LOCATION=$LOCATION/database_$NAME.txt
CERT_SERIAL_NUMBER_LOCATION=$LOCATION/serial_$NAME.txt
CERT_CRL_NUMBER_LOCATION=$LOCATION/crlnumber_$NAME.txt

# remove files to create when they already exist
if [ -f "$PRIVATE_KEY_LOCATION" ]; then rm "$PRIVATE_KEY_LOCATION" ; fi
if [ -f "$CERTIFICATE_LOCATION" ]; then rm "$CERTIFICATE_LOCATION" ; fi
if [ -f "$TEMP_CERTIFICATE_LOCATION" ]; then rm "$TEMP_CERTIFICATE_LOCATION" ; fi
if [ -f "$REQUEST_LOCATION" ]; then rm "$REQUEST_LOCATION" ; fi
if [ -f "$CERT_DATABASE_LOCATION" ]; then rm "$CERT_DATABASE_LOCATION"* ; fi
if [ -f "$CERT_SERIAL_NUMBER_LOCATION" ]; then rm "$CERT_SERIAL_NUMBER_LOCATION"* ; fi
if [ -f "$CERT_CRL_NUMBER_LOCATION" ]; then rm "$CERT_CRL_NUMBER_LOCATION"* ; fi

mkdir -p "$LOCATION"
mkdir -p "$LOCATION/private"
mkdir -p "$LOCATION/certs"
mkdir -p "$LOCATION/crls"
mkdir -p "$LOCATION/request"

# Generate private key
openssl genrsa -des3 -out "$PRIVATE_KEY_LOCATION" -passout pass:pass 2048
# Generate request key
openssl req -config "$CONFIGURATION" -new -key "$PRIVATE_KEY_LOCATION" -outform PEM -out "$REQUEST_LOCATION" -passin pass:pass
# Sign request, certificate in PEM format will be generated
openssl ca -config "$CONFIGURATION" -batch -days 365 -in "$REQUEST_LOCATION" -out "$TEMP_CERTIFICATE_LOCATION" -subj "$SUBJ" -passin pass:pass -cert "$SIGNING_CA_PEM_CERTIFICATE" -keyfile "$SIGNING_CA_PRIVATE_KEY"
# Convert PEM certificate to DER format
openssl x509 -inform PEM -in "$TEMP_CERTIFICATE_LOCATION" -outform DER -out "$CERTIFICATE_LOCATION"

