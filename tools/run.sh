#!/usr/bin/env bash

usageDisplayed=0
usage()
{
	if [[ "${usageDisplayed}" -eq 0 ]]; then
		echo "usage: $0 source_image source_dir [-h|--help] [-t|--time]"
		echo "  Builds and runs the mosaix executable with 'source_image' and 'source_dir' as inputs."
		echo "options:"
		echo "  -h|--help    Prints this message."
		echo "  -t|--time    Time the execution."
		usageDisplayed=1
	fi
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
sourceDir=

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
		elif [[ -z "${sourceDir}" ]]; then
			sourceDir="$arg"
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

if [[ -z "${sourceDir}" ]]; then
	echo "No source directory provided." 1>&2
	echo
	usage
	exit 1
fi

if [[ ! -d "${sourceDir}" ]]; then
	echo "Cannot find source directory '${sourceDir}'." 1>&2
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
execute "${execName}" "${sourceImage}" "${sourceDir}"
