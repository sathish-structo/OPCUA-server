#! /bin/bash

usage() {
  echo usage $0 PKI-dir
  exit 1
}

if [ $# -ne 1 ]
then
  usage
fi

TRUSTED_DIR=$1/trusted
ISSUER_DIR=$1/issuer
CRL_DIR=$1/crls

TRUSTED_IDX=$1/trusted_index
ISSUER_IDX=$1/issuer_index
CRL_IDX=$1/crl_index

if [ ! -d $TRUSTED_DIR ]
then
  usage
fi

echo creating PKI index...

rm -rf $ISSUER_IDX
rm -rf $TRUSTED_IDX
rm -rf $CRL_IDX

mkdir $ISSUER_IDX
if [ $? -ne 0 ]
then
  usage
fi

mkdir $TRUSTED_IDX
if [ $? -ne 0 ]
then
  usage
fi

mkdir $CRL_IDX
if [ $? -ne 0 ]
then
  usage
fi

for CRT in $TRUSTED_DIR/*.der
do
  H=`openssl x509 -inform der -in $CRT -noout -hash 2>/dev/null`
  if [ -n "$H" ]
  then
    K=0
    while [ -f $TRUSTED_IDX/$H.$K ]; do K=$((K+1)); done
    openssl x509 -inform der -in $CRT -outform der -out $TRUSTED_IDX/$H.$K
  fi
done

for CRT in $ISSUER_DIR/*.der
do
  H=`openssl x509 -inform der -in $CRT -noout -hash 2>/dev/null`
  if [ -n "$H" ]
  then
    K=0
    while [ -f $ISSUER_IDX/$H.$K ]; do K=$((K+1)); done
    openssl x509 -inform der -in $CRT -outform der -out $ISSUER_IDX/$H.$K
  fi
done

for CRL in $CRL_DIR/*.crl
do
  for FMT in pem der
  do
    H=`openssl crl -inform $FMT -in $CRL -noout -hash 2>/dev/null`
    if [ -n "$H" ]
    then
      K=0
      while [ -f $CRL_IDX/$H.r$K ]; do K=$((K+1)); done
      openssl crl -inform $FMT -in $CRL -outform pem -out $CRL_IDX/$H.r$K
    fi
  done
done

echo done.
