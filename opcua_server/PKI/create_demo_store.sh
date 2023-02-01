#!/bin/bash

pushd `dirname $0` > /dev/null 
CURRENTDIR="$PWD"
popd > /dev/null

if [ -d "$CURRENTDIR/bin" ]
then
	PATH="$CURRENTDIR/bin:$PATH"
fi

bash "$CURRENTDIR/batches/create_demo_store.sh" "$@"
