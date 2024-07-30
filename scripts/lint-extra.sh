#!/bin/bash

set -e

SCRIPT_PATH=$(readlink -f "$0")
SCRIPT_ROOT=$(dirname "$SCRIPT_PATH")

source "$SCRIPT_ROOT"/lint-all.sh
source "$SCRIPT_ROOT"/lint-pvs-studio.sh
