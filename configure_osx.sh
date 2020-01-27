#!/usr/bin/env bash

set -ex

which -s brew
if [[ $? != 0 ]] ; then
    echo Install Homebrew
    echo 'ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"'
    exit 0
fi

if [[ $(brew list | grep jq) != "jq" ]]; then
	brew install jq
fi;


PATHS="${HOME}/Library/Application Support/OpenVR/.openvr/openvrpaths.vrpath"
cat "${PATHS}"
RUNTIME=`jq -r '.runtime[0]' "${PATHS}"`
STEAMVRCONFIG_DIR=`jq -r '.config[0]' "${PATHS}"`
DRIVER_BASE_DIR="${RUNTIME}/drivers/northstar/"
DRIVER_DIR="${DRIVER_BASE_DIR}/bin/osx32"

mkdir -p "${DRIVER_DIR}"
rm -rf "${DRIVER_DIR}/*.dylib"
rm -rf "${DRIVER_DIR}/resources"
cp bin/osx32/northstar/*.dylib "${DRIVER_DIR}/"
cp -r driver/resources/* "${DRIVER_BASE_DIR}/"

if [ -f "${STEAMVRCONFIG_DIR}/steamvr.vrsettings" ]; then
    echo merge
    #cp launcher/steamvrconfig.json "${STEAMVRCONFIG_DIR}/steamvr.vrsettings"
    #jq -s 'add' "launcher/steamvrconfig.json" "${STEAMVRCONFIG_DIR}/steamvr.vrsettings" > "${STEAMVRCONFIG_DIR}/steamvr.vrsettings.tmp"
    jq -s '.[0] * .[1]' "${STEAMVRCONFIG_DIR}/steamvr.vrsettings" "launcher/steamvrconfig.json" > "${STEAMVRCONFIG_DIR}/steamvr.vrsettings.tmp"
    mv "${STEAMVRCONFIG_DIR}/steamvr.vrsettings.tmp" "${STEAMVRCONFIG_DIR}/steamvr.vrsettings"
else
    cp launcher/steamvrconfig.json "${STEAMVRCONFIG_DIR}/steamvr.vrsettings"
fi
