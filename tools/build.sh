#!/usr/bin/env bash

rootDir=$(cd "$(dirname $0)/../" ; pwd)

# Sanity check
if [[ ! -d "${rootDir}" || "$(basename ${rootDir})" != "mosaix" ]]; then
	echo "Incorrect root directory '${rootDir}'." 1>&2
	exit 1
fi

buildDir="$rootDir/build"

if [[ ! -d "$buildDir" ]]; then
	mkdir "$buildDir" ||
	exit 1
fi

cd "$buildDir"
cmake .. && make
