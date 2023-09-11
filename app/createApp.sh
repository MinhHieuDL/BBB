#!/bin/bash

# This script create application directory with the structure below:
#|app_name
#├── build
#├── include
#└── src
#└── CMakeLists.txt

# Check if user already supplied app name  
NUM_OF_PAR="${#}"
if [[ $NUM_OF_PAR -eq 0 ]]
then 
    echo 'Please supply app name'
    exit 1
fi

# Get app name
APP_NAME="$1"

# Check if app name already exists, then create the application directory structure
if [[ ! -d "$APP_NAME" ]]; then 
    mkdir -p "$APP_NAME"
    mkdir -p "$APP_NAME/build"
    mkdir -p "$APP_NAME/include"
    mkdir -p "$APP_NAME/src"
    touch "$APP_NAME/CMakeLists.txt"
else
    echo "Application with name: \"$APP_NAME\" already exists!"
    exit 1
fi
