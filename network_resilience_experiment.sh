#!/bin/bash

# Input data: 
# $1 = game networking library
# $2 = number of clients
# $3 = how long metrics need to be monitored and generated
# $4 = output directory
# $5 = turn on reliable messages, don't provide any input to keep it turned off

# Declare an array to store PIDs
declare -a PIDS

# Specify the LIBRARY
LIBRARY=$1

# Specify the number of clients
NUM_CLIENTS=$2

# set duration for metrics logging
DURATION=$3

# Set output directory
OUTPUT_DIR=$4

# Turn on reliable messages, just leave it blank or write 'reliable=true'
RELIABLE_MESSAGES=""
if [ "$5" = "reliable=true" ];
then
	RELIABLE_MESSAGES="-r"
	echo "Reliable messages turned on"
fi;

# Build the binaries
cd $LIBRARY
source build.sh
cd ..

# Define the network conditioner profiles to run through
NETWORK_CONDITIONER_PROFILES=("base" "10%pl_0ms" "25%pl_0ms" "50%pl_0ms" "75%pl_0ms" "0%pl_20ms" "0%pl_50ms" "0%pl_100ms" "0%pl_200ms" "25%pl_50ms" "50%pl_100ms")

for profile in ${NETWORK_CONDITIONER_PROFILES[@]}
do
	# Tell user to change profile
	echo "Change profile to $profile"

	# Wait for the profile change to take effect
	sleep 10 

	echo "Running for $DURATION seconds with profile $profile"

	# start the server
	(./$LIBRARY/server_bin -c $NUM_CLIENTS $RELIABLE_MESSAGES > /dev/null)  &

	# start the clients
	for ((i=1; i<=$NUM_CLIENTS; i++)); do
	    (./$LIBRARY/client_bin -x -p $OUTPUT_DIR/$profile $RELIABLE_MESSAGES > /dev/null)  &
	done

	# wait for processes to start
	sleep 1

	# create array of process ids for server and clients
	PIDS=($(pgrep -f "client_bin"))
	SERVER_BIN_PID=($(pgrep -o -f "server_bin"))
	PIDS=("$SERVER_BIN_PID" "${PIDS[@]}")

	# logging for the specified duration of time
	sleep $DURATION

	# kill all processes
	for PID in ${PIDS[@]}; do
		echo "Killing $PID"
		kill -9 $PID > /dev/null
	done 
done
