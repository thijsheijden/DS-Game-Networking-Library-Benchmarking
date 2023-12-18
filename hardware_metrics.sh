#!/bin/bash

# Input data: 
# $1 = game networking library
# $2 = number of clients
# $3 = how long metrics need to be monitored and generated
# $4 = turn on reliable messages, don't provide any input to keep it turned off

# Output directory
OUTPUT_DIR="usage_logs"

# Create the output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# Declare an array to store PIDs
declare -a PIDS

# Specify the LIBRARY
LIBRARY=$1

# Specify the number of clients
NUM_CLIENTS=$2

# set duration for metrics logging
DURATION=$3

# Turn on reliable messages, just leave it blank or write 'reliable=true'
RELIABLE_MESSAGES=""
if [ "$4" = "reliable=true" ];
then
	RELIABLE_MESSAGES="-R"
	echo "Reliable messages turned on"
fi;

# Build the binaries
cd $LIBRARY
source build.sh
cd ..

# start the server

(open -a Terminal ./$LIBRARY/server_bin $RELIABLE_MESSAGES > /dev/null)  &

# start the clients
for ((i=1; i<=$NUM_CLIENTS; i++)); do
    (open -a Terminal ./$LIBRARY/client_bin $RELIABLE_MESSAGES > /dev/null)  &
done

# wait for processes to start
sleep 10

# create array of process ids for server and clients
PIDS=($(pgrep -f "client_bin"))
SERVER_BIN_PID=($(pgrep -o -f "server_bin"))
PIDS=("$SERVER_BIN_PID" "${PIDS[@]}")

# start logging metrics
for PID in ${PIDS[@]}; do
    if [ "$PID" = "$SERVER_BIN_PID" ]; then
      OUTPUT_FILE="$OUTPUT_DIR/${LIBRARY}_${PID}_server_usage_log.csv"
    else
      OUTPUT_FILE="$OUTPUT_DIR/${LIBRARY}_${PID}_client_usage_log.csv"
    fi
    echo "Timestamp,CPU(%),RAM(%)" > "$OUTPUT_FILE"
    echo "Process is running with PID: $PID"
    while kill -0 "$PID" > /dev/null 2>&1; do
        CURRENT_TIME=$(date "+%Y-%m-%d %H:%M:%S")
        STATS=$(ps -p "$PID" -o %cpu,%mem | tail -n 1)
        CPU_USAGE=$(echo "$STATS" | awk '{print $1}')
        RAM_USAGE=$(echo "$STATS" | awk '{print $2}')
        # Append data to the CSV file
        echo "$CURRENT_TIME,$STATS" >> "$OUTPUT_FILE"
        sleep 1
    done &
done

# logging for the specified duration of time
sleep $DURATION

# kill all processes
for PID in ${PIDS[@]}; do
	echo "Killing $PID"
	kill -9 $PID > /dev/null
done