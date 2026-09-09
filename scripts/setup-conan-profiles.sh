#!/usr/bin/env bash
# Same layout as qbs/qbs scripts/setup-conan-profiles.sh, plus Linux.
# Profiles match prebuilt assimp/6.0.5 on Conan Center.
set -eu

ARCH=$(uname -m)
case "$ARCH" in
    x86_64)
        ARCH=x64
        ;;
    arm64|aarch64)
        ARCH=arm64
        ;;
esac

case "$OSTYPE" in
    *darwin*)
        HOST_OS=mac_$ARCH
        TOOLCHAIN=clang_64
        ;;
    msys|cygwin)
        HOST_OS=win_x64
        TOOLCHAIN=msvc_64
        ;;
    linux*)
        HOST_OS=linux_$ARCH
        TOOLCHAIN=gcc_64
        ;;
    *)
        HOST_OS=
        ;;
esac

while [ $# -gt 0 ]; do
    case "$1" in
        --host)
            HOST_OS="$2"
            shift
            ;;
        --toolchain)
            TOOLCHAIN=$(echo $2 | tr '[A-Z]' '[a-z]')
            shift
            ;;
    esac
    shift
done

if [ -z "${HOST_OS}" ]; then
    echo "No --host specified or auto-detection failed." >&2
    exit 1
fi

if [ -z "${TOOLCHAIN}" ]; then
    echo "No --toolchain specified or auto-detection failed." >&2
    exit 1
fi

echo $ARCH
echo $HOST_OS
echo $TOOLCHAIN

mkdir -p "${HOME}/.conan2/profiles"
SCRIPT_DIR=$( cd "$(dirname "$0")" ; pwd -P )
cp ${SCRIPT_DIR}/conan-profiles/${HOST_OS}/${TOOLCHAIN}/* "${HOME}/.conan2/profiles"
