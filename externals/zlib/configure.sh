#!/bin/bash

echo "Checking for bash..."
if [ -z "$BASH_VERSION" ]; then
	echo "Invalid shell, re-running using bash..."
	exec bash "$0" "$@"
	exit $?
fi
SRCLOC="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source "$SRCLOC/../functions.sh"

configureExternalFromTarArchive "$SRCLOC" "http://download.osmand.net/prebuilt/zlib-1.2.8.tar.bz2"
patchExternal "$SRCLOC"
