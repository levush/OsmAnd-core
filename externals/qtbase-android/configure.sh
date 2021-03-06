#!/bin/bash

echo "Checking for bash..."
if [ -z "$BASH_VERSION" ]; then
	echo "Invalid shell, re-running using bash..."
	exec bash "$0" "$@"
	exit $?
fi
SRCLOC="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source "$SRCLOC/../functions.sh"

configureExternalFromGit "$SRCLOC" "https://github.com/osmandapp/OsmAnd-external-qtbase.git" "qt-v5.3.0-rc1"
patchExternal "$SRCLOC"
