#!/bin/bash

# Write data to /dev/scull0
echo -n "Hello, scull!" > /dev/scull0

# Read data from /dev/scull0
output=$(cat /dev/scull0)

# Display the read data
echo "Read from scull: $output"
