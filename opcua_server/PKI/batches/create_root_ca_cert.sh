#!/bin/bash
# Usage create_root_ca_cert.sh 'location_root' 'ca_name' 'subj'
# where ca_name is the filename without file extention (e.g. myCA)
# and subj has the form: "/C=%COUNTRY%/L=%LOCATION%/ST=%STATE%/O=%ORGANIZATION%/OU=%ORGANIZATION_UNIT%/CN=%TO%"

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
	echo "No subject has been specified"
	exit 1
fi

LOCATION=$1
CA_NAME=$2
SUBJ=$3
echo $SUBJ

CONFIGURATION=$BATCH_DIR/openssl.cnf
CA_PRIVATE_KEY_LOCATION=$LOCATION/private/private_key_$CA_NAME.pem
CA_REQUEST_LOCATION=$LOCATION/request/req_$CA_NAME.csr
CA_PEM_CERTIFICATE_LOCATION=$LOCATION/certs/cert_$CA_NAME.pem
CA_DER_CERTIFICATE_LOCATION=$LOCATION/certs/cert_$CA_NAME.der

# These three variables are used by the OpenSSL as environment variables
CA_DATABASE_LOCATION=$LOCATION/database_$CA_NAME.txt
CA_SERIAL_NUMBER_LOCATION=$LOCATION/serial_$CA_NAME.txt
CA_CRL_NUMBER_LOCATION=$LOCATION/crlnumber_$CA_NAME.txt
export CA_DATABASE_LOCATION
export CA_SERIAL_NUMBER_LOCATION
export CA_CRL_NUMBER_LOCATION

# remove files to create when they already exist
if [ -f "$CA_PRIVATE_KEY_LOCATION" ]; then rm "$CA_PRIVATE_KEY_LOCATION" ; fi
if [ -f "$CA_REQUEST_LOCATION" ]; then rm "$CA_REQUEST_LOCATION" ; fi
if [ -f "$CA_PEM_CERTIFICATE_LOCATION" ]; then rm "$CA_PEM_CERTIFICATE_LOCATION" ; fi
if [ -f "$CA_DER_CERTIFICATE_LOCATION" ]; then rm "$CA_DER_CERTIFICATE_LOCATION" ; fi
if [ -f "$CA_DATABASE_LOCATION" ]; then rm "$CA_DATABASE_LOCATION"* ; fi
if [ -f "$CA_SERIAL_NUMBER_LOCATION" ]; then rm "$CA_SERIAL_NUMBER_LOCATION"* ; fi
if [ -f "$CA_CRL_NUMBER_LOCATION" ]; then rm "$CA_CRL_NUMBER_LOCATION"* ; fi

# Initialize location

mkdir -p "$LOCATION"
mkdir -p "$LOCATION/private"
mkdir -p "$LOCATION/certs"
mkdir -p "$LOCATION/crls"
mkdir -p "$LOCATION/request"

# Creating CA related files
touch "$CA_DATABASE_LOCATION"
echo 00 > "$CA_SERIAL_NUMBER_LOCATION"
echo 00 > "$CA_CRL_NUMBER_LOCATION"

# Creating the files using OpenSSL
# Generate private key
openssl genrsa -des3 -out "$CA_PRIVATE_KEY_LOCATION" -passout pass:pass 2048 
# Generate request key
openssl req -config "$CONFIGURATION" -new -key "$CA_PRIVATE_KEY_LOCATION" -outform PEM -out "$CA_REQUEST_LOCATION" -passin pass:pass
# Sign request, certificate in PEM format will be generated
openssl ca -config "$CONFIGURATION" -batch -days 3650 -in "$CA_REQUEST_LOCATION" -extensions v3_ca -out "$CA_PEM_CERTIFICATE_LOCATION" -subj "$SUBJ" -passin pass:pass -selfsign -keyfile "$CA_PRIVATE_KEY_LOCATION"
# Convert PEM certificate to DER format
openssl x509 -inform PEM -in "$CA_PEM_CERTIFICATE_LOCATION" -outform DER -out "$CA_DER_CERTIFICATE_LOCATION"

