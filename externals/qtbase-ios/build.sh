#!/bin/bash

echo "Checking for bash..."
if [ -z "$BASH_VERSION" ]; then
	echo "Invalid shell, re-running using bash..."
	exec bash "$0" "$@"
	exit $?
fi
SRCLOC="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

QTBASE_CONFIGURATION=$(echo "
	-release -opensource -confirm-license -c++11 -static -largefile -no-accessibility -qt-sql-sqlite
	-no-qml-debug -qt-zlib -no-gif -no-libpng -no-libjpeg -no-openssl -qt-pcre
	-nomake examples -nomake tools -no-gui -no-widgets -no-nis -no-cups -no-iconv -no-icu -no-dbus
	-no-xcb -no-eglfs -no-directfb -no-linuxfb -no-kms -no-opengl -no-glib
	-v
" | tr '\n' ' ')

if [[ "$(uname -a)" =~ Darwin ]]; then
	if [[ -z "$OSMAND_BUILD_CPU_CORES_NUM" ]]; then
		OSMAND_BUILD_CPU_CORES_NUM=`sysctl hw.ncpu | awk '{print $2}'`
	fi

	# Function: makeFlavor(name, platform, configuration)
	makeFlavor()
	{
		local name=$1
		local platform=$2
		local configuration=$3
		
		local path="$SRCLOC/upstream.patched.$name"
		
		# Configure
		if [ ! -d "$path" ]; then
			cp -rpf "$SRCLOC/upstream.patched" "$path"
			(cd "$path" && ./configure -xplatform $platform $configuration)
			retcode=$?
			if [ $retcode -ne 0 ]; then
				echo "Failed to configure 'qtbase-ios' for '$name', aborting..."
				rm -rf "$path"
				exit $retcode
			fi
		fi
		
		# Build
		(cd "$path" && make -j$OSMAND_BUILD_CPU_CORES_NUM)
		retcode=$?
		if [ $retcode -ne 0 ]; then
			echo "Failed to build 'qtbase-ios' for '$name', aborting..."
			rm -rf "$path"
			exit $retcode
		fi
	}
	
	makeFlavor "ios.simulator.i386.static" "macx-ios-clang-simulator-i386" "$QTBASE_CONFIGURATION -sdk iphonesimulator"
	makeFlavor "ios.device.armv7.static" "macx-ios-clang-device-armv7" "$QTBASE_CONFIGURATION -sdk iphoneos"
	makeFlavor "ios.device.armv7s.static" "macx-ios-clang-device-armv7s" "$QTBASE_CONFIGURATION -sdk iphoneos"

	if [ ! -h "$SRCLOC/upstream.patched.ios.simulator.static" ]; then
		ln -s "$SRCLOC/upstream.patched.ios.simulator.i386.static" "$SRCLOC/upstream.patched.ios.simulator.static"
	fi

	if [ ! -h "$SRCLOC/upstream.patched.ios.device.static" ]; then
		ln -s "$SRCLOC/upstream.patched.ios.device.armv7.static" "$SRCLOC/upstream.patched.ios.device.static"
	fi

	if [ ! -h "$SRCLOC/upstream.patched.ios-iphoneos" ]; then
		ln -s "$SRCLOC/upstream.patched.ios.device.armv7.static" "$SRCLOC/upstream.patched.ios-iphoneos"
	fi

	if [ ! -h "$SRCLOC/upstream.patched.ios-iphonesimulator" ]; then
		ln -s "$SRCLOC/upstream.patched.ios.simulator.i386.static" "$SRCLOC/upstream.patched.ios-iphonesimulator"
	fi

	if [ ! -d "$SRCLOC/upstream.patched.ios" ]; then
		# Make link to cmake stuff and includes from already built target (any is suitable)
		mkdir -p "$SRCLOC/upstream.patched.ios/lib"
		ln -s "$SRCLOC/upstream.patched.ios.simulator.i386.static/lib/cmake" "$SRCLOC/upstream.patched.ios/lib/cmake"
		ln -s "$SRCLOC/upstream.patched.ios.simulator.i386.static/include" "$SRCLOC/upstream.patched.ios/include"
		ln -s "$SRCLOC/upstream.patched.ios.simulator.i386.static/bin" "$SRCLOC/upstream.patched.ios/bin"

		# Make universal libraries using lipo
		libraries=(Core Concurrent Network Sql Xml)
		for libName in "${libraries[@]}" ; do
			echo "Packing '$libName'..."
			lipo -create \
				"$SRCLOC/upstream.patched.ios.simulator.i386.static/lib/libQt5${libName}.a" \
				"$SRCLOC/upstream.patched.ios.device.armv7.static/lib/libQt5${libName}.a" \
				"$SRCLOC/upstream.patched.ios.device.armv7s.static/lib/libQt5${libName}.a" \
				-output "$SRCLOC/upstream.patched.ios/lib/libQt5${libName}.a"
			retcode=$?
			if [ $retcode -ne 0 ]; then
				echo "Failed to lipo 'libQt5${libName}.a', aborting..."
				rm -rf "$path"
				exit $retcode
			fi
		done
	fi
	if [ ! -d "$SRCLOC/upstream.patched.ios.fat.static" ]; then
		mkdir -p "$SRCLOC/upstream.patched.ios.fat.static"
		ln -s "$SRCLOC/upstream.patched.ios/lib" "$SRCLOC/upstream.patched.ios.fat.static/lib"
		ln -s "$SRCLOC/upstream.patched.ios/include" "$SRCLOC/upstream.patched.ios.fat.static/include"
		ln -s "$SRCLOC/upstream.patched.ios/bin" "$SRCLOC/upstream.patched.ios.fat.static/bin"
	fi
fi
