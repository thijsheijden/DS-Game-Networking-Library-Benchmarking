#!/bin/bash

# Input data: 
# $1 = game networking library
# $2 = how long metrics need to be monitored and generated
# $3 = turn on reliable messages, don't provide any input to keep it turned off

# Number of clients to benchmark
NUM_CLIENTS=(2 4 8 16 32 64)

OUTPUT_DIR_EXTENSION=""
if [ "$3" = "reliable=true" ];
then
	OUTPUT_DIR_EXTENSION="_reliable"
	echo "Reliable messages turned on"
fi;

# Run hardware metrics for the given number of clients
for nc in ${NUM_CLIENTS[@]}; do
	OUTPUT_DIR="usage_logs/${1}/scalability_experiment/${nc}_clients${OUTPUT_DIR_EXTENSION}"
	./hardware_metrics.sh $1 $nc $2 $OUTPUT_DIR $3
done;
