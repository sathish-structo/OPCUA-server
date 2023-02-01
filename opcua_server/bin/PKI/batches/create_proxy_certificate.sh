#!/bin/bash
# Usage create_proxy_certificate.sh 'location_root' 'signing_certificate_name' 'subj'
# where signing_certificate_name is the filename without file extention (e.g. client2)
# and subj has the form: "/C=%COUNTRY%/DC=%DomainComponent%/ST=%STATE%/O=%ORGANIZATION%/OU=%ORGANIZATION_UNIT%/CN=%TO%"

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

LOCATION=$1
SIGNING_NAME=$2
SUBJ=$3

CONFIGURATION=$BATCH_DIR/openssl.cnf
PROXY_NAME="$SIGNING_NAME"_proxy

SIGNING_PRIVATE_KEY_LOCATION=$LOCATION/private/private_key_$SIGNING_NAME.pem
SIGNING_TEMP_CERTIFICATE_LOCATION=$LOCATION/certs/cert_$SIGNING_NAME.pem
SIGNING_CERTIFICATE_LOCATION=$LOCATION/certs/cert_$SIGNING_NAME.der
# These three variables are used by the OpenSSL as environment variables
CA_DATABASE_LOCATION=$LOCATION/database_$SIGNING_NAME.txt
CA_SERIAL_NUMBER_LOCATION=$LOCATION/serial_$SIGNING_NAME.txt
CA_CRL_NUMBER_LOCATION=$LOCATION/crlnumber_$SIGNING_NAME.txt
export CA_DATABASE_LOCATION
export CA_SERIAL_NUMBER_LOCATION
export CA_CRL_NUMBER_LOCATION

PROXY_PRIVATE_KEY_LOCATION=$LOCATION/private/private_key_$PROXY_NAME.pem
PROXY_TEMP_PEM_CERTIFICATE_LOCATION=$LOCATION/certs/cert_"$PROXY_NAME"_part.pem
PROXY_TEMP_DER_CERTIFICATE_LOCATION=$LOCATION/certs/cert_"$PROXY_NAME"_part.der
PROXY_CERTIFICATE_LOCATION=$LOCATION/certs/cert_$PROXY_NAME.der
PROXY_REQUEST_LOCATION=$LOCATION/request/req_$PROXY_NAME.csr

# remove files to create when they already exist
if [ -f "$PROXY_PRIVATE_KEY_LOCATION" ]; then rm "$PROXY_PRIVATE_KEY_LOCATION" ; fi
if [ -f "$PROXY_TEMP_PEM_CERTIFICATE_LOCATION" ]; then rm "$PROXY_TEMP_PEM_CERTIFICATE_LOCATION" ; fi
if [ -f "$PROXY_TEMP_DER_CERTIFICATE_LOCATION" ]; then rm "$PROXY_TEMP_DER_CERTIFICATE_LOCATION" ; fi
if [ -f "$PROXY_CERTIFICATE_LOCATION" ]; then rm "$PROXY_CERTIFICATE_LOCATION" ; fi
if [ -f "$PROXY_REQUEST_LOCATION" ]; then rm "$PROXY_REQUEST_LOCATION" ; fi

mkdir -p "$LOCATION"
mkdir -p "$LOCATION/private"
mkdir -p "$LOCATION/certs"
mkdir -p "$LOCATION/crls"
mkdir -p "$LOCATION/request"

# not self signed certificates usually don't have a data base and a serial number file. Create them

if [ ! -f "$CA_DATABASE_LOCATION" ];
then
	# Generate an empty file
	touch "$CA_DATABASE_LOCATION"
fi

if [ ! -f "$CA_SERIAL_NUMBER_LOCATION" ];
then
	echo 00 > "$CA_SERIAL_NUMBER_LOCATION"
fi

# Generate private key
openssl genrsa -des3 -out "$PROXY_PRIVATE_KEY_LOCATION"  -passout pass:pass 2048 
# Generate request key
openssl req -config "$CONFIGURATION" -new -key "$PROXY_PRIVATE_KEY_LOCATION" -outform PEM -out "$PROXY_REQUEST_LOCATION" -passin pass:pass
# Sign request, certificate in PEM format will be generated
openssl ca -config "$CONFIGURATION" -batch -days 365 -in "$PROXY_REQUEST_LOCATION" -extensions v3_proxy -out "$PROXY_TEMP_PEM_CERTIFICATE_LOCATION" -subj "$SUBJ" -passin pass:pass -cert "$SIGNING_TEMP_CERTIFICATE_LOCATION" -keyfile "$SIGNING_PRIVATE_KEY_LOCATION" -preserveDN
# Convert PEM certificate to DER format
openssl x509 -inform PEM -in "$PROXY_TEMP_PEM_CERTIFICATE_LOCATION" -outform DER -out "$PROXY_TEMP_DER_CERTIFICATE_LOCATION"
# Attach the issuer certificate for chain validation
cat "$PROXY_TEMP_DER_CERTIFICATE_LOCATION" "$SIGNING_CERTIFICATE_LOCATION" > "$PROXY_CERTIFICATE_LOCATION"
rm $PROXY_TEMP_DER_CERTIFICATE_LOCATION
