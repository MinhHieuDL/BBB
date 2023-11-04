#!/bin/bash

usage(){
    echo "Usage: ${0} [-c TARGET] [-b TARGET]"
    echo "Build full remote login app"
    echo ' -c TARGET Clean previous build of target' 
    echo ' -b TARGET Generate build file and perform build target' 
    echo " Available target: all, client, tool, server"
    exit 1 
}

log(){
    local MESSAGE="${@}"
    if [[ "${VERBOSE}" = 'true' ]]
    then 
        echo "${MESSAGE}"
    fi
}

# List of target directories
TARGETS=("client" "tool" "server")

# Project root directories
PR_ROOT_DIR="$(pwd)"

# Clean action define
clean(){
    local CLEAN_DIR="${PR_ROOT_DIR}/${1}/build"
    log "cleaning target: ${1}"
    if [[ -d "$CLEAN_DIR" ]]; then 
        rm -rvf "$CLEAN_DIR"
    fi
    log "clean done"
}

cleanHandler(){
    local target="${1}"

    if [[ "$target" == "all" ]]; then
        log 'cleaning all sub-applications'
        for APP_DIR in "{TARGETS[@]}"; do 
            clean "$APP_DIR"
        done
    else
        clean "$APP_DIR"
    fi
}

# build action define
build(){
    log "start building target: ${1}"
    local APP_DIR="${PR_ROOT_DIR}/${1}"

    # Create a build directory if it doesn't exist
    local BUILD_DIR="${APP_DIR}/build"
    mkdir -p "$BUILD_DIR"

    # Run CMake to generate build files
    cmake -B "$BUILD_DIR" "$APP_DIR"

    # Run make to build the application
    make -C "$BUILD_DIR"
    log "build target: ${1} done"
}

buildHandler(){
    local target="${1}"

    if [[ "$target" == "all" ]]; then
        log 'build all sub-applications'
        for APP in "{TARGETS[@]}"; do 
            build "$APP_DIR"
        done
    else
        build "$target"
    fi
}