#!/usr/bin/env bash

#set -ex

ICDS=($(ls /usr/share/vulkan/icd.d/))
LEN=${#ICDS[@]}
SELECTION=0
while (( SELECTION < 1 || SELECTION > LEN )); do
    COUNT=0
    echo "Vulkan ICDs:"
    echo
    for ICD in ${ICDS[@]}; do
        COUNT=$((COUNT+1))

        echo "[${COUNT}] ${ICD}"
    done
    echo
    echo "Please enter a selection: "
    read SELECTION
    ICD="/usr/share/vulkan/icd.d/${ICDS[$SELECTION-1]}"
done

OFFSET_X=0
OFFSET_Y=0
echo
echo "Enter Northstar Headset X Offset [0]: "
read OFFSET_X
echo "Enter Northstar Headset Y Offset [0]: "
read OFFSET_Y

OFFSET_X=${OFFSET_X:-0}
OFFSET_Y=${OFFSET_Y:-0}

PACKAGES="steam steam-devices libx11-dev cmake jq mesa-vulkan-drivers mesa-vulkan-drivers:i386 vulkan-utils"
MISSING_COUNT=$(dpkg-query -l ${PACKAGES} 2>&1 | grep -i "no packages" | wc -l)
if [[ ${MISSING_COUNT} != "0" ]]; then
sudo DEBIAN_FRONTEND=noninteractive apt install -y ${PACKAGES}
fi

cmake -B build .
cmake --build ./build

PATHS="${HOME}/.config/openvr/openvrpaths.vrpath"
#cat "${PATHS}"
RUNTIME=`jq -r '.runtime[0]' "${PATHS}"`
STEAMVRCONFIG_DIR=`jq -r '.config[0]' "${PATHS}"`
DRIVER_BASE_DIR="${RUNTIME}/drivers/northstar/"
DRIVER_DIR="${DRIVER_BASE_DIR}/bin/linux64"

mkdir -p "${DRIVER_DIR}"
rm -rf "${DRIVER_DIR}/*.so"
rm -rf "${DRIVER_DIR}/resources"
cp bin/linux64/northstar/*.so "${DRIVER_DIR}/"
cp -r driver/resources/* "${DRIVER_BASE_DIR}/"

touch "${STEAMVRCONFIG_DIR}/steamvr.vrsettings"
if [ ! -s "$_file" ]; then
    cp launcher/steamvrconfig.json "${STEAMVRCONFIG_DIR}/steamvr.vrsettings"
fi

jq -s '.[0] * .[1]' "${STEAMVRCONFIG_DIR}/steamvr.vrsettings" "launcher/steamvrconfig.json" > "${STEAMVRCONFIG_DIR}/steamvr.vrsettings.tmp"
jq ".driver_northstar.headsetwindowX=${OFFSET_X} | .driver_northstar.headsetwindowY=${OFFSET_Y}" "${STEAMVRCONFIG_DIR}/steamvr.vrsettings.tmp" > "${STEAMVRCONFIG_DIR}/steamvr.vrsettings"

echo "export VK_ICD_FILENAMES=${ICD}" | sudo tee /etc/profile.d/steamvr-vulkan.sh > /dev/null
