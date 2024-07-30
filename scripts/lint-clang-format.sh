#!/bin/bash

set -e

SCRIPT_PATH=$(readlink -f "$0")
SCRIPT_ROOT=$(dirname "$SCRIPT_PATH")
PROJECT_ROOT=$(dirname "$SCRIPT_ROOT")
SOURCES_ROOT="$PROJECT_ROOT/sources"
INCLUDE_ROOT="$PROJECT_ROOT/include"
SAMPLES_ROOT="$PROJECT_ROOT/samples"

find "$SOURCES_ROOT" "$INCLUDE_ROOT" "$SAMPLES_ROOT" \
    -regex '.+\.[hc]pp' \
    -exec clang-format-15 --dry-run -Werror {} +;
