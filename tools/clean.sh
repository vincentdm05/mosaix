#!/usr/bin/env bash

rootDir=$(cd "$(dirname $0)/../" ; pwd)

# Sanity check
if [[ ! -d "${rootDir}" || "$(basename ${rootDir})" != "mosaix" ]]; then
	echo "Incorrect root directory '${rootDir}'." 1>&2
	exit 1
fi

binDir="${rootDir}/bin"
buildDir="${rootDir}/build"

rm -r "${binDir}" 2>/dev/null && echo "Removed '${binDir}'"
rm -r "${buildDir}" 2>/dev/null && echo "Removed '${buildDir}'" 
