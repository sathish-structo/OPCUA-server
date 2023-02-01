#!/bin/bash

pushd `dirname $0` > /dev/null
BATCH_DIR="$PWD"
popd > /dev/null

DEFAULT_STORE_PATH=$BATCH_DIR/../all_certificates
export OPENSSL_CONF=$BATCH_DIR/openssl.cnf
export RANDFILE=$DEFAULT_STORE_PATH/.rnd

usage() {
	echo -e "This batch file creates certificates and corresponding private keys of the sample client and server applications delivered with the OPC UA Toolkit."
	echo -e ""
	echo -e "Usage: `basename $0` [-a|-c <Host name> <Domain> <IP address>] [-o <Output directory>]"
	echo -e ""
	echo -e "-a\tUse the hostname command to guess"
	echo -e "\thost name, domain name and IP address"
	echo -e "\tto create instance certificates."
	echo -e ""
	echo -e "-c\tUse the given host name, domain name and IP address"
	echo -e "\tto create instance certificates. If this parameter is"
	echo -e "\tnot specified, the configuration is performed interactively."
	echo -e ""
	echo -e "-o\tSpecifies the output directory of the generated files."
	echo -e "\tBy default, the generated files are placed in"
	echo -e "\t$DEFAULT_STORE_PATH"
	echo -e ""
	echo -e "-ecc\tSpecifies curve for the EC certificates."
	echo -e "\tPossible choices: none, nistP256, nistP384, brainpoolP256r1 and brainpoolP384r1."
	echo -e "\tBy default, the EC certifiates are generated using nistP256."
	echo -e ""
	echo -e "-ecx\tSpecifies curve for the ED certificates."
	echo -e "\tPossible choices: none, curve25519 and curve448."
	echo -e "\tBy default, no ED certificates are generated."

	exit 1
}

# Generates the subject using the Common Name and the Hostname and stores the result in 'X509_SUBJ'
# setSubject 'Common Name' 'Hostname'
setSubject() {
	X509_SUBJ="/C=DE/L=Haar (Munich)/ST=Bayern/O=Softing IA GmbH/OU=IA/CN=""$1""/emailAddress=support.automation@softing.com/DC=""$2"
}

# Parse batch parameters
INTERACTIVE=true

HOSTNAME=
DOMAINNAME=
IPADDR=
STORE_PATH=$DEFAULT_STORE_PATH
EC_CURVE=none
ED_CURVE=none

while [ $# -gt 0 ]; do
	case $1 in
		-a)
			INTERACTIVE=false
			HOSTNAME=`hostname -s`
			DOMAINNAME=`hostname -d`
			IPADDRS=`hostname -I`
			IPADDR=""
			# hostname may return multiple IP addrs
			for IP in $IPADDRS;
			do
				if [ "$IPADDR" == "" ]
				then
					if [ "$IP" != "0.0.0.0" ]
					then
						IPADDR="$IP"
					fi
				fi
			done
			if [ "$IPADDR" == "" ]
			then
				IPADDR="127.0.0.1"
			fi
		;;

		-c) if [ "$#" -lt 4 ]; then
				usage
			fi

			INTERACTIVE=false
			HOSTNAME=$2
			DOMAINNAME=$3
			IPADDR=$4
			shift 3
		;;

		-o) if [ "$#" -lt 2 ]; then
				usage
			fi

			STORE_PATH=$2
			shift
		;;

		-ecc) if [ "$#" -lt 2 ]; then
				usage
			fi

			EC_CURVE=$2
			shift
		;;

		-ecx) if [ "$#" -lt 2 ]; then
				usage
			fi

			ED_CURVE=$2
			shift
		;;

		*)  usage
		;;
		esac

		shift
done

if [ "$INTERACTIVE" = "true" ]
	then
		echo Please enter your hostname:
		read HOSTNAME

		echo
		echo Please enter your domain name or leave it empty for no domain:
		read DOMAINNAME

		echo
		echo Please enter your IP address:
		read IPADDR
fi

case "$EC_CURVE" in
	nistP256)
		EC_CURVE=prime256v1
	;;
	nistP384)
		EC_CURVE=secp384r1
	;;
	*)
	;;
esac

case "$ED_CURVE" in
	curve25519)
		ED_CURVE=ed25519
	;;
	curve448)
		ED_CURVE=ed448
	;;
	*)
	;;
esac

mkdir -p "$STORE_PATH"

# Do not include the domain name if the host does not belong to a domain
if [ "$DOMAINNAME" != "" ]
then
	FQDN=$HOSTNAME.$DOMAINNAME
else
	FQDN=$HOSTNAME
fi

SERVER_ALTERNATIVE_SUBJECT=URI:urn:$HOSTNAME/Softing/OpcUa/StructoAutomoldServer,DNS:$FQDN,IP:$IPADDR
CLIENT_ALTERNATIVE_SUBJECT=URI:urn:$HOSTNAME/Softing/OpcUa/TestClient

# separate subject for sample applications
SERVER_SAMPLE_SUBJECT=URI:urn:$HOSTNAME/Softing/OpcUa/SampleServer,DNS:$FQDN,IP:$IPADDR
CLIENT_SAMPLE_SUBJECT=URI:urn:$HOSTNAME/Softing/OpcUa/SampleClient

# This variable is required to be set because it is referenced from the OpenSSL config file.
SOFTING_CERT_GENERATION_URI=$SERVER_ALTERNATIVE_SUBJECT
export SOFTING_CERT_GENERATION_URI
export STORE_PATH

# These three variables are used by the OpenSSL as environment variables
CA_DATABASE_LOCATION=$LOCATION/database_default.txt
CA_SERIAL_NUMBER_LOCATION=$LOCATION/serial_default.txt
CA_CRL_NUMBER_LOCATION=$LOCATION/crlnumber_default.txt
export CA_DATABASE_LOCATION
export CA_SERIAL_NUMBER_LOCATION
export CA_CRL_NUMBER_LOCATION

openssl version

# =====================================
# = Creating root CA
# =====================================
setSubject "Softing OpcUa Test CA" "$HOSTNAME"
bash "$BATCH_DIR/create_root_ca_cert.sh" "$STORE_PATH" root_ca "$X509_SUBJ"

# =====================================
# = Creating intermediate CA
# =====================================
setSubject "Softing Demo Intermediate CA" "$HOSTNAME"
bash "$BATCH_DIR/create_intermediate_ca_cert.sh" "$STORE_PATH" root_ca inter_ca "$X509_SUBJ"


# =====================================
# = Creating server certificate
# =====================================
setSubject "Softing Test Server" "$HOSTNAME"
bash "$BATCH_DIR/create_certificate.sh" "$STORE_PATH" inter_ca server "$X509_SUBJ"

# =====================================
# = Creating second server certificate
# =====================================
setSubject "Softing Test Server2" "$HOSTNAME"
bash "$BATCH_DIR/create_certificate.sh" "$STORE_PATH" inter_ca server2 "$X509_SUBJ"

# =====================================
# = Revoking second server certificate
# =====================================
bash "$BATCH_DIR/revoke_certificate.sh" "$STORE_PATH" inter_ca server2

# This variable is required to be set because it is referenced from the OpenSSL config file.
SOFTING_CERT_GENERATION_URI=$CLIENT_ALTERNATIVE_SUBJECT
export SOFTING_CERT_GENERATION_URI

# =====================================
# = Creating client certificate
# =====================================
setSubject "Softing Test Client" "$HOSTNAME"
bash "$BATCH_DIR/create_certificate.sh" "$STORE_PATH" inter_ca client "$X509_SUBJ"

# =====================================
# = Creating proxy certificate of client certificate
# =====================================
# Proxy certificates policy has an extra constraint, where the
# certificate subject MUST be the issuer with a single CN entry
# added. (RFC 3820: 3.4, 4.1.3 (a)(4))
X509_SUBJ="/C=DE/ST=Bayern/O=Softing IA GmbH/OU=IA/CN=Softing Test Client/L=Haar (Munich)/emailAddress=support.automation@softing.com/DC=$HOSTNAME/CN=Softing Test Client Proxy"
bash "$BATCH_DIR/create_proxy_certificate.sh" "$STORE_PATH" client "$X509_SUBJ"

# =====================================
# = Creating second client certificate
# =====================================
setSubject "Softing Test Client2" "$HOSTNAME"
bash "$BATCH_DIR/create_certificate.sh" "$STORE_PATH" inter_ca client2 "$X509_SUBJ"

# =====================================
# = Revoking second client certificate
# =====================================
bash "$BATCH_DIR/revoke_certificate.sh" "$STORE_PATH" inter_ca client2

# =====================================
# = Updating CRL of intermediate CA
# =====================================
bash "$BATCH_DIR/update_revokation_list.sh" "$STORE_PATH" inter_ca


# This variable is required to be set because it is referenced from the OpenSSL config file.
SOFTING_CERT_GENERATION_URI=$SERVER_ALTERNATIVE_SUBJECT
export SOFTING_CERT_GENERATION_URI

# =====================================
# = Creating second intermediate CA
# =====================================
setSubject "Softing Demo Intermediate CA 2" "$HOSTNAME"
bash "$BATCH_DIR/create_intermediate_ca_cert.sh" "$STORE_PATH" root_ca inter_ca2 "$X509_SUBJ"

# =====================================
# = Revoking second intermediate CA
# =====================================
bash "$BATCH_DIR/revoke_certificate.sh" "$STORE_PATH" root_ca inter_ca2

# =====================================
# = Creating third server certificate
# =====================================
setSubject "Softing Test Server3" "$HOSTNAME"
bash "$BATCH_DIR/create_certificate.sh" "$STORE_PATH" inter_ca2 server3 "$X509_SUBJ"

# =====================================
# = Creating fourth server certificate
# =====================================
setSubject "Softing Test Server4" "$HOSTNAME"
bash "$BATCH_DIR/create_certificate.sh" "$STORE_PATH" inter_ca2 server4 "$X509_SUBJ"

# =====================================
# = Revoking fourth server certificate
# =====================================
bash "$BATCH_DIR/revoke_certificate.sh" "$STORE_PATH" inter_ca2 server4

# This variable is required to be set because it is referenced from the OpenSSL config file.
SOFTING_CERT_GENERATION_URI=$CLIENT_ALTERNATIVE_SUBJECT
export SOFTING_CERT_GENERATION_URI

# =====================================
# = Creating third client certificate
# =====================================
setSubject "Softing Test Client3" "$HOSTNAME"
bash "$BATCH_DIR/create_certificate.sh" "$STORE_PATH" inter_ca2 client3 "$X509_SUBJ"

# =====================================
# = Creating proxy certificate of third client certificate
# =====================================
X509_SUBJ="/C=DE/ST=Bayern/O=Softing IA GmbH/OU=IA/CN=Softing Test Client3/L=Haar (Munich)/emailAddress=support.automation@softing.com/DC=$HOSTNAME/CN=Softing Test Client3 Proxy"
bash "$BATCH_DIR/create_proxy_certificate.sh" "$STORE_PATH" client3 "$X509_SUBJ"

# =====================================
# = Creating fourth client certificate
# =====================================
setSubject "Softing Test Client4" "$HOSTNAME"
bash "$BATCH_DIR/create_certificate.sh" "$STORE_PATH" inter_ca2 client4 "$X509_SUBJ"

# =====================================
# = Revoking fourth client certificate
# =====================================
bash "$BATCH_DIR/revoke_certificate.sh" "$STORE_PATH" inter_ca2 client4

# =====================================
# = Updating CRL of revoked intermediate CA
# =====================================
bash "$BATCH_DIR/update_revokation_list.sh" "$STORE_PATH" inter_ca2


# =====================================
# = Updating CRL of root CA
# =====================================
bash "$BATCH_DIR/update_revokation_list.sh" "$STORE_PATH" root_ca


# This variable is required to be set because it is referenced from the OpenSSL config file.
SOFTING_CERT_GENERATION_URI=$SERVER_ALTERNATIVE_SUBJECT
export SOFTING_CERT_GENERATION_URI

# =====================================
# = Generating server self signed certificate
# =====================================
setSubject "Softing OpcUa Test Server (self signed)" "$HOSTNAME"
bash "$BATCH_DIR/create_self_signed_certificate.sh" "$STORE_PATH" server_self_signed "$X509_SUBJ"

# =====================================
# = Updating CRL of server self signed
# =====================================
bash "$BATCH_DIR/update_revokation_list.sh" "$STORE_PATH" server_self_signed

if [ "$EC_CURVE" != "none" ]; then
# =====================================
# = Generating server self signed EC certificate
# =====================================
setSubject "Softing OpcUa Test Server (self signed EC)" "$HOSTNAME"
bash "$BATCH_DIR/create_self_signed_ec_certificate.sh" "$STORE_PATH" server_self_signed_ec "$X509_SUBJ" $EC_CURVE

# =====================================
# = Updating CRL of server self signed EC
# =====================================
bash "$BATCH_DIR/update_revokation_list.sh" "$STORE_PATH" server_self_signed_ec
fi

if [ "$ED_CURVE" != "none" ]; then
# =====================================
# = Generating server self signed ED certificate
# =====================================
setSubject "Softing OpcUa Test Server (self signed ED)" "$HOSTNAME"
bash "$BATCH_DIR/create_self_signed_ed_certificate.sh" "$STORE_PATH" server_self_signed_ed "$X509_SUBJ" $ED_CURVE

# =====================================
# = Updating CRL of server self signed ED
# =====================================
bash "$BATCH_DIR/update_revokation_list.sh" "$STORE_PATH" server_self_signed_ed
fi

# This variable is required to be set because it is referenced from the OpenSSL config file.
SOFTING_CERT_GENERATION_URI=$CLIENT_ALTERNATIVE_SUBJECT
export SOFTING_CERT_GENERATION_URI

# =====================================
# = Generating client self signed certificate
# =====================================
setSubject "Softing OpcUa Test Client (self signed)" "$HOSTNAME"
bash "$BATCH_DIR/create_self_signed_certificate.sh" "$STORE_PATH" client_self_signed "$X509_SUBJ"

# =====================================
# = Updating CRL of client self signed
# =====================================
bash "$BATCH_DIR/update_revokation_list.sh" "$STORE_PATH" client_self_signed

if [ "$EC_CURVE" != "none" ]; then
# =====================================
# = Generating client self signed EC certificate
# =====================================
setSubject "Softing OpcUa Test Client (self signed EC)" "$HOSTNAME"
bash "$BATCH_DIR/create_self_signed_ec_certificate.sh" "$STORE_PATH" client_self_signed_ec "$X509_SUBJ" $EC_CURVE

# =====================================
# = Updating CRL of client self signed EC
# =====================================
bash "$BATCH_DIR/update_revokation_list.sh" "$STORE_PATH" client_self_signed_ec
fi

if [ "$ED_CURVE" != "none" ]; then
# =====================================
# = Generating client self signed ED certificate
# =====================================
setSubject "Softing OpcUa Test Client (self signed ED)" "$HOSTNAME"
bash "$BATCH_DIR/create_self_signed_ed_certificate.sh" "$STORE_PATH" client_self_signed_ed "$X509_SUBJ" $ED_CURVE

# =====================================
# = Updating CRL of client self signed ED
# =====================================
bash "$BATCH_DIR/update_revokation_list.sh" "$STORE_PATH" client_self_signed_ed
fi

# Create certificates for sample
# This variable is required to be set because it is referenced from the OpenSSL config file.
SOFTING_CERT_GENERATION_URI=$CLIENT_SAMPLE_SUBJECT
export SOFTING_CERT_GENERATION_URI

# =====================================
# = Generating sample client certificate
# =====================================
setSubject "Softing OpcUa Sample Client" "$HOSTNAME"
bash "$BATCH_DIR/create_self_signed_certificate.sh" "$STORE_PATH" sample_client "$X509_SUBJ"

# =====================================
# = Updating CRL of sample client
# =====================================
bash "$BATCH_DIR/update_revokation_list.sh" "$STORE_PATH" sample_client

# This variable is required to be set because it is referenced from the OpenSSL config file.
SOFTING_CERT_GENERATION_URI=$SERVER_SAMPLE_SUBJECT
export SOFTING_CERT_GENERATION_URI

# =====================================
# = Generating server certificate
# =====================================
setSubject "Softing OpcUa Sample Server" "$HOSTNAME"
bash "$BATCH_DIR/create_self_signed_certificate.sh" "$STORE_PATH" sample_server "$X509_SUBJ"

# =====================================
# = Updating CRL of server certificate
# =====================================
bash "$BATCH_DIR/update_revokation_list.sh" "$STORE_PATH" sample_server

# =====================================
# = Create DH parameters for the server
# =====================================
# no longer necessary since elyptic curves are enabled in OpenSSL
#bash "$BATCH_DIR/create_dhparam_file.sh" "$STORE_PATH"

# Now create the PKI stores for test client and server
CLIENT_STORE_PATH=$DEFAULT_STORE_PATH/../test_client
if [ ! -d $CLIENT_STORE_PATH ]; then mkdir $CLIENT_STORE_PATH; fi
if [ ! -d $CLIENT_STORE_PATH/trusted ]; then mkdir $CLIENT_STORE_PATH/trusted; fi
if [ ! -d $CLIENT_STORE_PATH/own ]; then mkdir $CLIENT_STORE_PATH/own; fi
if [ ! -d $CLIENT_STORE_PATH/issuer ]; then mkdir $CLIENT_STORE_PATH/issuer; fi
if [ ! -d $CLIENT_STORE_PATH/rejected ]; then mkdir $CLIENT_STORE_PATH/rejected; fi
if [ ! -d $CLIENT_STORE_PATH/trusted_crls ]; then mkdir $CLIENT_STORE_PATH/trusted_crls; fi
SERVER_STORE_PATH=$DEFAULT_STORE_PATH/../test_server
if [ ! -d $SERVER_STORE_PATH ]; then mkdir $SERVER_STORE_PATH; fi
if [ ! -d $SERVER_STORE_PATH/trusted ]; then mkdir $SERVER_STORE_PATH/trusted; fi
if [ ! -d $SERVER_STORE_PATH/own ]; then mkdir $SERVER_STORE_PATH/own; fi
if [ ! -d $SERVER_STORE_PATH/issuer ]; then mkdir $SERVER_STORE_PATH/issuer; fi
if [ ! -d $SERVER_STORE_PATH/rejected ]; then mkdir $SERVER_STORE_PATH/rejected; fi
if [ ! -d $SERVER_STORE_PATH/trusted_crls ]; then mkdir $SERVER_STORE_PATH/trusted_crls; fi

cp -f $DEFAULT_STORE_PATH/certs/cert_server_self_signed.der $CLIENT_STORE_PATH/trusted/cert_server_self_signed.der
cp -f $DEFAULT_STORE_PATH/certs/cert_client_self_signed.der $CLIENT_STORE_PATH/own/cert_client_self_signed.der
cp -f $DEFAULT_STORE_PATH/private/private_key_client_self_signed.pem $CLIENT_STORE_PATH/own/private_key_client_self_signed.pem

cp -f $DEFAULT_STORE_PATH/certs/cert_client_self_signed.der $SERVER_STORE_PATH/trusted/cert_client_self_signed.der
cp -f $DEFAULT_STORE_PATH/certs/cert_server_self_signed.der $SERVER_STORE_PATH/own/cert_server_self_signed.der
cp -f $DEFAULT_STORE_PATH/private/private_key_server_self_signed.pem $SERVER_STORE_PATH/own/private_key_server_self_signed.pem
# no longer necessary since elyptic curves are enabled in OpenSSL
#cp -f $DEFAULT_STORE_PATH/private/dhparam.pem $SERVER_STORE_PATH/own/dhparam.pem

if [ "$EC_CURVE" != "none" ]; then
# Shuffle EC certificates around
cp -f $DEFAULT_STORE_PATH/certs/cert_server_self_signed_ec.der $CLIENT_STORE_PATH/trusted/cert_server_self_signed_ec.der
cp -f $DEFAULT_STORE_PATH/certs/cert_client_self_signed_ec.der $CLIENT_STORE_PATH/own/cert_client_self_signed_ec.der
cp -f $DEFAULT_STORE_PATH/private/private_key_client_self_signed_ec.pem $CLIENT_STORE_PATH/own/private_key_client_self_signed_ec.pem

cp -f $DEFAULT_STORE_PATH/certs/cert_client_self_signed_ec.der $SERVER_STORE_PATH/trusted/cert_client_self_signed_ec.der
cp -f $DEFAULT_STORE_PATH/certs/cert_server_self_signed_ec.der $SERVER_STORE_PATH/own/cert_server_self_signed_ec.der
cp -f $DEFAULT_STORE_PATH/private/private_key_server_self_signed_ec.pem $SERVER_STORE_PATH/own/private_key_server_self_signed_ec.pem
fi

if [ "$ED_CURVE" != "none" ]; then
# Shuffle ED certificates around
cp -f $DEFAULT_STORE_PATH/certs/cert_server_self_signed_ed.der $CLIENT_STORE_PATH/trusted/cert_server_self_signed_ed.der
cp -f $DEFAULT_STORE_PATH/certs/cert_client_self_signed_ed.der $CLIENT_STORE_PATH/own/cert_client_self_signed_ed.der
cp -f $DEFAULT_STORE_PATH/private/private_key_client_self_signed_ed.pem $CLIENT_STORE_PATH/own/private_key_client_self_signed_ed.pem

cp -f $DEFAULT_STORE_PATH/certs/cert_client_self_signed_ed.der $SERVER_STORE_PATH/trusted/cert_client_self_signed_ed.der
cp -f $DEFAULT_STORE_PATH/certs/cert_server_self_signed_ed.der $SERVER_STORE_PATH/own/cert_server_self_signed_ed.der
cp -f $DEFAULT_STORE_PATH/private/private_key_server_self_signed_ed.pem $SERVER_STORE_PATH/own/private_key_server_self_signed_ed.pem
fi

# Now create the PKI stores for sample client and server
CLIENT_STORE_PATH=$DEFAULT_STORE_PATH/../sample_client
if [ ! -d $CLIENT_STORE_PATH ]; then mkdir $CLIENT_STORE_PATH; fi
if [ ! -d $CLIENT_STORE_PATH/trusted ]; then mkdir $CLIENT_STORE_PATH/trusted; fi
if [ ! -d $CLIENT_STORE_PATH/own ]; then mkdir $CLIENT_STORE_PATH/own; fi
if [ ! -d $CLIENT_STORE_PATH/issuer ]; then mkdir $CLIENT_STORE_PATH/issuer; fi
if [ ! -d $CLIENT_STORE_PATH/rejected ]; then mkdir $CLIENT_STORE_PATH/rejected; fi
if [ ! -d $CLIENT_STORE_PATH/trusted_crls ]; then mkdir $CLIENT_STORE_PATH/trusted_crls; fi
SERVER_STORE_PATH=$DEFAULT_STORE_PATH/../sample_server
if [ ! -d $SERVER_STORE_PATH ]; then mkdir $SERVER_STORE_PATH; fi
if [ ! -d $SERVER_STORE_PATH/trusted ]; then mkdir $SERVER_STORE_PATH/trusted; fi
if [ ! -d $SERVER_STORE_PATH/own ]; then mkdir $SERVER_STORE_PATH/own; fi
if [ ! -d $SERVER_STORE_PATH/issuer ]; then mkdir $SERVER_STORE_PATH/issuer; fi
if [ ! -d $SERVER_STORE_PATH/rejected ]; then mkdir $SERVER_STORE_PATH/rejected; fi
if [ ! -d $SERVER_STORE_PATH/trusted_crls ]; then mkdir $SERVER_STORE_PATH/trusted_crls; fi

cp -f $DEFAULT_STORE_PATH/certs/cert_sample_server.der $CLIENT_STORE_PATH/trusted/cert_sample_server.der
cp -f $DEFAULT_STORE_PATH/certs/cert_sample_client.der $CLIENT_STORE_PATH/own/cert_sample_client.der
cp -f $DEFAULT_STORE_PATH/private/private_key_sample_client.pem $CLIENT_STORE_PATH/own/private_key_sample_client.pem

cp -f $DEFAULT_STORE_PATH/certs/cert_sample_client.der $SERVER_STORE_PATH/trusted/cert_sample_client.der
cp -f $DEFAULT_STORE_PATH/certs/cert_sample_server.der $SERVER_STORE_PATH/own/cert_sample_server.der
cp -f $DEFAULT_STORE_PATH/private/private_key_sample_server.pem $SERVER_STORE_PATH/own/private_key_sample_server.pem

# Clean-up intermediate files
find $DEFAULT_STORE_PATH -maxdepth 1 -type f -exec rm {} \;
rm $DEFAULT_STORE_PATH/certs/*.pem
rm -rf $DEFAULT_STORE_PATH/request
