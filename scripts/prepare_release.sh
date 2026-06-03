#!/usr/bin/env bash
# prepare_release.sh
# Prepares a ThreadX release by updating version constants and port version strings.
#
# Usage:   prepare_release.sh <version>
# Example: prepare_release.sh 6.5.1.202602
# Hotfix:  prepare_release.sh 6.5.1.202602a
#
# This script:
#   1. Creates branch release-<version>-preparation from dev
#   2. Updates version constants in common/inc/tx_api.h
#      (commit: "Updated version number constants")
#   3. Updates port version strings in all tx_port.h files
#      (commit: "Updated port version strings")
#
# Copyright (C) 2026 Eclipse ThreadX contributors
# SPDX-License-Identifier: MIT

set -eu

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

API_HEADER="${REPO_ROOT}/common/inc/tx_api.h"
PORT_HEADER_NAME="tx_port.h"
PORT_DIRS="ports ports_smp ports_arch ports_module"

# --------------------------------------------------------------------------
# Argument validation
# --------------------------------------------------------------------------
if [ "$#" -ne 1 ]; then
    printf "Usage: %s <version>\n" "$(basename "$0")" >&2
    printf "Example: %s 6.5.1.202602\n" "$(basename "$0")" >&2
    exit 1
fi

VERSION="$1"

if ! printf "%s" "${VERSION}" | grep -qE '^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+[a-z]?$'; then
    printf "Error: Invalid version format '%s'.\n" "${VERSION}" >&2
    printf "Expected: MAJOR.MINOR.PATCH.BUILD[hotfix_letter]\n" >&2
    exit 1
fi

# --------------------------------------------------------------------------
# Parse version components
# --------------------------------------------------------------------------
MAJOR=$(printf "%s" "${VERSION}" | cut -d. -f1)
MINOR=$(printf "%s" "${VERSION}" | cut -d. -f2)
PATCH=$(printf "%s" "${VERSION}" | cut -d. -f3)
BUILD_AND_HOTFIX=$(printf "%s" "${VERSION}" | cut -d. -f4)
BUILD=$(printf "%s" "${BUILD_AND_HOTFIX}" | sed -E 's/[a-z]+$//')
HOTFIX=$(printf "%s" "${BUILD_AND_HOTFIX}" | sed -E 's/^[0-9]+//')

if [ -z "${HOTFIX}" ]; then
    HOTFIX_DEFINE="' '"
else
    HOTFIX_DEFINE="'${HOTFIX}'"
fi

# --------------------------------------------------------------------------
# Read and display current version
# --------------------------------------------------------------------------
CURR_MAJOR=$(grep -E "^#define THREADX_MAJOR_VERSION" "${API_HEADER}" | awk '{print $NF}')
CURR_MINOR=$(grep -E "^#define THREADX_MINOR_VERSION" "${API_HEADER}" | awk '{print $NF}')
CURR_PATCH=$(grep -E "^#define THREADX_PATCH_VERSION" "${API_HEADER}" | awk '{print $NF}')
CURR_BUILD=$(grep -E "^#define THREADX_BUILD_VERSION" "${API_HEADER}" | awk '{print $NF}')
CURR_HOTFIX=$(grep -E "^#define THREADX_HOTFIX_VERSION" "${API_HEADER}" | \
              sed -E "s/.*'([^']*)'.*/\1/" | tr -d ' ')

if [ -z "${CURR_HOTFIX}" ]; then
    CURR_VER="${CURR_MAJOR}.${CURR_MINOR}.${CURR_PATCH}.${CURR_BUILD}"
else
    CURR_VER="${CURR_MAJOR}.${CURR_MINOR}.${CURR_PATCH}.${CURR_BUILD}${CURR_HOTFIX}"
fi

printf "\nThreadX release preparation\n"
printf "  Repository      : %s\n" "${REPO_ROOT}"
printf "  Current version : %s\n" "${CURR_VER}"
printf "  Target version  : %s\n\n" "${VERSION}"
printf "Proceed with update? [y/N] "
read -r CONFIRM
case "${CONFIRM}" in
    y|Y) ;;
    *)
        printf "Aborted.\n"
        exit 0
        ;;
esac

# --------------------------------------------------------------------------
# Pre-flight checks
# --------------------------------------------------------------------------
if ! git -C "${REPO_ROOT}" diff --quiet HEAD 2>/dev/null; then
    printf "Error: Working tree has uncommitted changes. Commit or stash first.\n" >&2
    exit 1
fi

# --------------------------------------------------------------------------
# Create feature branch from dev
# --------------------------------------------------------------------------
BRANCH_NAME="release-${VERSION}-preparation"
printf "\nChecking out dev and pulling latest changes...\n"
git -C "${REPO_ROOT}" checkout dev
git -C "${REPO_ROOT}" pull origin dev
printf "Creating branch '%s'...\n" "${BRANCH_NAME}"
git -C "${REPO_ROOT}" checkout -b "${BRANCH_NAME}"

# --------------------------------------------------------------------------
# Update version constants
# --------------------------------------------------------------------------
printf "\nUpdating version constants in %s...\n" "${API_HEADER}"

sed -i -E "s|(#define THREADX_MAJOR_VERSION[[:space:]]+)[0-9]+|\1${MAJOR}|" "${API_HEADER}"
sed -i -E "s|(#define THREADX_MINOR_VERSION[[:space:]]+)[0-9]+|\1${MINOR}|" "${API_HEADER}"
sed -i -E "s|(#define THREADX_PATCH_VERSION[[:space:]]+)[0-9]+|\1${PATCH}|" "${API_HEADER}"
sed -i -E "s|(#define THREADX_BUILD_VERSION[[:space:]]+)[0-9]+|\1${BUILD}|" "${API_HEADER}"
sed -i -E "s|(#define THREADX_HOTFIX_VERSION[[:space:]]+)'[^']*'|\1${HOTFIX_DEFINE}|" "${API_HEADER}"

git -C "${REPO_ROOT}" add "${API_HEADER}"
git -C "${REPO_ROOT}" commit -F - <<'COMMIT_EOF'
Updated version number constants

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>
COMMIT_EOF

printf "Committed version constant updates.\n"

# --------------------------------------------------------------------------
# Update port version strings
# --------------------------------------------------------------------------
printf "\nUpdating port version strings...\n"

PORT_FILES=""
for dir in ${PORT_DIRS}; do
    if [ -d "${REPO_ROOT}/${dir}" ]; then
        found=$(find "${REPO_ROOT}/${dir}" -name "${PORT_HEADER_NAME}" 2>/dev/null | sort)
        if [ -n "${found}" ]; then
            PORT_FILES="${PORT_FILES}${found}
"
        fi
    fi
done
PORT_FILES=$(printf "%s" "${PORT_FILES}" | grep -v '^[[:space:]]*$' || true)

if [ -z "${PORT_FILES}" ]; then
    printf "Warning: No port header files found. Skipping port version string commit.\n"
else
    while IFS= read -r port_file; do
        if [ -n "${port_file}" ] && grep -qE "Version [0-9]" "${port_file}" 2>/dev/null; then
            sed -i -E "s/Version [0-9]+\.[0-9]+\.[0-9]+\.[0-9]+[a-z]*/Version ${VERSION}/g" "${port_file}"
            printf "  Updated: %s\n" "${port_file#${REPO_ROOT}/}"
        fi
    done <<EOF
${PORT_FILES}
EOF

    git -C "${REPO_ROOT}" add -u
    if git -C "${REPO_ROOT}" diff --cached --quiet; then
        printf "No port version string changes staged. Skipping commit.\n"
    else
        git -C "${REPO_ROOT}" commit -F - <<'COMMIT_EOF'
Updated port version strings

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>
COMMIT_EOF
        printf "Committed port version string updates.\n"
    fi
fi

printf "\nRelease preparation complete.\n"
printf "Branch '%s' is ready for review.\n" "${BRANCH_NAME}"
