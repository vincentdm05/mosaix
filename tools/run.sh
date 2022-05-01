#!/usr/bin/env bash

usage()
{
	echo "usage: $0 source_image [-h|--help] [-t|--time]"
	echo "  Builds and runs the mosaix executable with 'source_image' as input."
	echo "options:"
	echo "  -h|--help    Prints this message."
	echo "  -t|--time    Time the execution."
}

timeExec=0
execute()
{
	if [[ $timeExec -eq 1 ]]; then
		time { $@ ; }
	else
		$@
	fi
}

sourceImage=

# Handle arguments
while [[ $# -gt 0 ]]; do
	arg="$1"
	case "$arg" in
		-h|--help)
		usage
		shift
		;;
		-t|--time)
		timeExec=1
		shift
		;;
		*)
		if [[ -z "${sourceImage}" ]]; then
			sourceImage="$arg"
		else
			echo "Unrecognized input \"$arg\" will be ignored." 1>&2
		fi
		shift
		;;
	esac
done

if [[ -z "${sourceImage}" ]]; then
	echo "No source image provided." 1>&2
	echo
	usage
	exit 1
fi

if [[ ! -f "${sourceImage}" ]]; then
	echo "Cannot find source image '${sourceImage}'." 1>&2
	exit 1
fi

rootDir=$(cd "$(dirname $0)/../" ; pwd)

# Sanity check
if [[ ! -d "${rootDir}" || "$(basename ${rootDir})" != "mosaix" ]]; then
	echo "Incorrect root directory '${rootDir}'." 1>&2
	exit 1
fi

toolDir="${rootDir}/tools"

# Build the code
( "${toolDir}/build.sh" ) || exit 1

execName="${rootDir}/bin/mosaix"

if [[ ! -f "${execName}" ]]; then
	echo "Executable does not exist. Did you forget to build it?" 1>&2
	exit 1
fi

# Run
execute "${execName}" "${sourceImage}" "sourceDir"
