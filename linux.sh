#!/usr/bin/env bash

set -ex

OFFSET_X=${1:-0}

PACKAGES="steam steam-devices libx11-dev cmake jq mesa-vulkan-drivers mesa-vulkan-drivers:i386 vulkan-utils"
MISSING_COUNT=$(dpkg-query -l ${PACKAGES} 2>&1 | grep -i "no packages" | wc -l)
if [[ ${MISSING_COUNT} != "0" ]]; then
sudo DEBIAN_FRONTEND=noninteractive apt install -y ${PACKAGES}
fi

cmake -B build .
cmake --build ./build

PATHS="${HOME}/.config/openvr/openvrpaths.vrpath"
cat "${PATHS}"
RUNTIME=`jq -r '.runtime[0]' "${PATHS}"`
STEAMVRCONFIG_DIR=`jq -r '.config[0]' "${PATHS}"`
DRIVER_BASE_DIR="${RUNTIME}/drivers/northstar/"
DRIVER_DIR="${DRIVER_BASE_DIR}/bin/linux64"

mkdir -p "${DRIVER_DIR}"
rm -rf "${DRIVER_DIR}/*.so"
rm -rf "${DRIVER_DIR}/resources"
cp bin/linux64/northstar/*.so "${DRIVER_DIR}/"
cp -r driver/resources/* "${DRIVER_BASE_DIR}/"

if [ ! -f "${STEAMVRCONFIG_DIR}/steamvr.vrsettings" ]; then
    cp launcher/steamvrconfig.json "${STEAMVRCONFIG_DIR}/steamvr.vrsettings"
fi

jq -s '.[0] * .[1]' "${STEAMVRCONFIG_DIR}/steamvr.vrsettings" "launcher/steamvrconfig.json" > "${STEAMVRCONFIG_DIR}/steamvr.vrsettings.tmp"
jq ".driver_northstar.headsetwindowX=${OFFSET_X}" "${STEAMVRCONFIG_DIR}/steamvr.vrsettings.tmp" > "${STEAMVRCONFIG_DIR}/steamvr.vrsettings"
