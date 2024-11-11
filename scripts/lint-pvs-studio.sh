#!/bin/bash

set -e

SCRIPT_PATH=$(readlink -f "$0")
SCRIPT_ROOT=$(dirname "$SCRIPT_PATH")
PROJECT_ROOT=$(dirname "$SCRIPT_ROOT")
BUILD_ROOT="$PROJECT_ROOT/_"

meson setup --wipe "$BUILD_ROOT" "$PROJECT_ROOT"

PVS_LOG_FILE="$PROJECT_ROOT/.pvs.log"
PVS_ERROR_FILE="$PROJECT_ROOT/.pvs.errors"

pvs-studio-analyzer analyze -f "$BUILD_ROOT/compile_commands.json" -o "$PVS_LOG_FILE" -j
plog-converter -a GA:1,2 -t errorfile-verbose -o "$PVS_ERROR_FILE" "$PVS_LOG_FILE" > /dev/null

rm "$PVS_LOG_FILE"

if [ -s "$PROJECT_ROOT/.pvs.errors" ]; then
    tail -n +2 "$PVS_ERROR_FILE" | sed -e "s/\([^:]*\):\([^:]*\):\([^:]*\):\([^:]*\):\(.*\)/\n$(tput bold)\1:\2:\3:$(tput setaf 1)\4:$(tput sgr0)$(tput bold)\5$(tput sgr0)/g"
    echo ""
    exit 1
else
    rm "$PVS_ERROR_FILE"
    exit 0
fi
