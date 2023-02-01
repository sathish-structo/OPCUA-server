#!/bin/bash
# Usage create_intermediate_ca_cert.sh 'location_root' 'signing_ca_name' 'target_ca_name' 'subj'
# where signing_ca_name and target_ca_name are the filename without file extention (e.g. myCA)
# and subj has the form: "/C=%COUNTRY%/L=%LOCATION%/ST=%STATE%/O=%ORGANIZATION%/OU=%ORGANIZATION_UNIT%/CN=%TO%"

pushd `dirname $0` > /dev/null
BATCH_DIR="$PWD"
popd > /dev/null

if [ "$#" -lt 1 ]; then
	echo "No path has been specified"
	exit 1
fi
if [ "$#" -lt 2 ]; then
	echo "No signing CA has been specified"
	exit 1
fi
if [ "$#" -lt 3 ]; then
	echo "No target CA has been specified"
	exit 1
fi
if [ "$#" -lt 4 ]; then
	echo "No subject has been specified"
	exit 1
fi

LOCATION=$1
SIGNING_CA_NAME=$2
TARGET_CA_NAME=$3
SUBJ=$4

CONFIGURATION=$BATCH_DIR/openssl.cnf
SIGNING_CA_PRIVATE_KEY_LOCATION=$LOCATION/private/private_key_$SIGNING_CA_NAME.pem
SIGNING_CA_DER_CERTIFICATE_LOCATION=$LOCATION/certs/cert_$SIGNING_CA_NAME.der
SIGNING_CA_PEM_CERTIFICATE_LOCATION=$LOCATION/certs/cert_$SIGNING_CA_NAME.pem
# These three variables are used by the OpenSSL as environment variables
CA_DATABASE_LOCATION=$LOCATION/database_$SIGNING_CA_NAME.txt
CA_SERIAL_NUMBER_LOCATION=$LOCATION/serial_$SIGNING_CA_NAME.txt
CA_CRL_NUMBER_LOCATION=$LOCATION/crlnumber_$SIGNING_CA_NAME.txt
export CA_DATABASE_LOCATION
export CA_SERIAL_NUMBER_LOCATION
export CA_CRL_NUMBER_LOCATION

TARGET_CA_PRIVATE_KEY_LOCATION=$LOCATION/private/private_key_$TARGET_CA_NAME.pem
TARGET_CA_REQUEST_LOCATION=$LOCATION/request/req_$TARGET_CA_NAME.csr
TARGET_CA_DER_CERTIFICATE_LOCATION=$LOCATION/certs/cert_$TARGET_CA_NAME.der
TARGET_CA_PEM_CERTIFICATE_LOCATION=$LOCATION/certs/cert_$TARGET_CA_NAME.pem
TARGET_DATABASE_LOCATION=$LOCATION/database_$TARGET_CA_NAME.txt
TARGET_SERIAL_NUMBER_LOCATION=$LOCATION/serial_$TARGET_CA_NAME.txt
TARGET_CRL_NUMBER_LOCATION=$LOCATION/crlnumber_$TARGET_CA_NAME.txt

# remove files to create when they already exist
if [ -f "$TARGET_CA_PRIVATE_KEY_LOCATION" ]; then rm "$TARGET_CA_PRIVATE_KEY_LOCATION" ; fi
if [ -f "$TARGET_CA_REQUEST_LOCATION" ]; then rm "$TARGET_CA_REQUEST_LOCATION" ; fi
if [ -f "$TARGET_CA_DER_CERTIFICATE_LOCATION" ]; then rm "$TARGET_CA_DER_CERTIFICATE_LOCATION" ; fi
if [ -f "$TARGET_CA_PEM_CERTIFICATE_LOCATION" ]; then rm "$TARGET_CA_PEM_CERTIFICATE_LOCATION" ; fi
if [ -f "$TARGET_DATABASE_LOCATION" ]; then rm "$TARGET_DATABASE_LOCATION"* ; fi
if [ -f "$TARGET_SERIAL_NUMBER_LOCATION" ]; then rm "$TARGET_SERIAL_NUMBER_LOCATION"* ; fi
if [ -f "$TARGET_CRL_NUMBER_LOCATION" ]; then rm "$TARGET_CRL_NUMBER_LOCATION"* ; fi

mkdir -p "$LOCATION"
mkdir -p "$LOCATION/private"
mkdir -p "$LOCATION/certs"
mkdir -p "$LOCATION/crls"
mkdir -p "$LOCATION/request"

# Create CA related files
# Generate an empty file
touch "$TARGET_DATABASE_LOCATION"
echo 00 > "$TARGET_SERIAL_NUMBER_LOCATION"
echo 00 > "$TARGET_CRL_NUMBER_LOCATION"

# Generate private key
openssl genrsa -des3 -out "$TARGET_CA_PRIVATE_KEY_LOCATION" -passout pass:pass 2048 
# Generate request key
openssl req -config "$CONFIGURATION" -new -key "$TARGET_CA_PRIVATE_KEY_LOCATION" -outform PEM -out "$TARGET_CA_REQUEST_LOCATION" -passin pass:pass
# Sign request, certificate in PEM format will be generated
openssl ca -config "$CONFIGURATION" -batch -days 3650 -in "$TARGET_CA_REQUEST_LOCATION" -extensions v3_ca -out "$TARGET_CA_PEM_CERTIFICATE_LOCATION" -subj "$SUBJ" -passin pass:pass -cert "$SIGNING_CA_PEM_CERTIFICATE_LOCATION" -keyfile "$SIGNING_CA_PRIVATE_KEY_LOCATION"
# Convert PEM certificate to DER format
openssl x509 -inform PEM -in "$TARGET_CA_PEM_CERTIFICATE_LOCATION" -outform DER -out "$TARGET_CA_DER_CERTIFICATE_LOCATION"

