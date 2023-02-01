#!/bin/bash
# Usage create_self_signed_certificate.sh 'location_root' 'unique_name' 'subj'
# where subj has the form: "/C=%COUNTRY%/L=%LOCATION%/ST=%STATE%/O=%ORGANIZATION%/OU=%ORGANIZATION_UNIT%/CN=%TO%"

pushd `dirname $0` > /dev/null
BATCH_DIR="$PWD"
popd > /dev/null

if [ "$#" -lt 1 ]; then
	echo "No path has been specified"
	exit 1
fi
if [ "$#" -lt 2 ]; then
	echo "No name has been specified"
	exit 1
fi
if [ "$#" -lt 3 ]; then
	echo "No subject has been specified"
	exit 1
fi
if [ "$#" -lt 4 ]; then
	echo "No curve has been specified"
	exit 1
fi

LOCATION=$1
NAME=$2
SUBJ=$3
CURVE=$4

CONFIGURATION=$BATCH_DIR/openssl.cnf
PRIVATE_KEY_LOCATION=$LOCATION/private/private_key_$NAME.pem
REQUEST_LOCATION=$LOCATION/request/req_$NAME.csr
CERTIFICATE_LOCATION=$LOCATION/certs/cert_$NAME.der
TEMP_CERTIFICATE_LOCATION=$LOCATION/certs/cert_$NAME.pem

# These three variables are used by the OpenSSL as environment variables
CA_DATABASE_LOCATION=$LOCATION/database_$NAME.txt
CA_SERIAL_NUMBER_LOCATION=$LOCATION/serial_$NAME.txt
CA_CRL_NUMBER_LOCATION=$LOCATION/crlnumber_$NAME.txt
export CA_DATABASE_LOCATION
export CA_SERIAL_NUMBER_LOCATION
export CA_CRL_NUMBER_LOCATION

# remove files to create when they already exist
if [ -f "$PRIVATE_KEY_LOCATION" ]; then rm "$PRIVATE_KEY_LOCATION" ; fi
if [ -f "$REQUEST_LOCATION" ]; then rm "$REQUEST_LOCATION" ; fi
if [ -f "$CERTIFICATE_LOCATION" ]; then rm "$CERTIFICATE_LOCATION" ; fi
if [ -f "$TEMP_CERTIFICATE_LOCATION" ]; then rm "$TEMP_CERTIFICATE_LOCATION" ; fi
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
# Generate an empty file
touch "$CA_DATABASE_LOCATION"
echo 00 > "$CA_SERIAL_NUMBER_LOCATION"
echo 00 > "$CA_CRL_NUMBER_LOCATION"

# Creating private key
openssl genpkey -algorithm $CURVE -aes256 -out "$PRIVATE_KEY_LOCATION" -pass pass:pass

# Creating certificate request
openssl req -config "$CONFIGURATION" -new -key "$PRIVATE_KEY_LOCATION" -outform PEM -out "$REQUEST_LOCATION" -passin pass:pass

# Creating self signed cert
openssl ca -config "$CONFIGURATION" -batch -days 365 -selfsign -keyfile "$PRIVATE_KEY_LOCATION" -in "$REQUEST_LOCATION" -out "$TEMP_CERTIFICATE_LOCATION" -subj "$SUBJ" -passin pass:pass -extensions v3_self_signed_ec

# Convert PEM certificate to DER format
openssl x509 -inform PEM -in "$TEMP_CERTIFICATE_LOCATION" -outform DER -out "$CERTIFICATE_LOCATION"

