# Arguments:
# $1 Library to benchmark
# $2 Number of clients
# $3 Test duration
# $4 Reliable messaging ("true" turns on reliable messaging)

library=$1
echo "Running network metrics benchmark for $library"

reliable_messages=""
if [ "$4" = "true" ];
then
	reliable_messages="-r"
	echo "Reliable messages turned on"
fi;

# Tracked started background processes
pids=()

# Build client and server binaries
cd $library
source build.sh
cd ..

# Start logging network traffic using tshark
output_file_name="network_metrics_"$library"_$(date +"%s").csv"
echo "Logging network metrics to $output_file_name"
tshark -i lo0 -f "port 60000" -T fields -e frame.time_relative -e frame.number -e frame.len -E header=y -E separator=, > "network_usage_benchmark_output/$output_file_name" &
tshark_pid=$!
echo "Started tshark ($tshark_pid) logging of packets to and from port 60,000"

# Start the server in background process
(./$library/server_bin $reliable_messages -c $2 > /dev/null) &
pids+=($!)
echo "Started server ($!)"

# Start the clients
 for ((i=1; i<=$2; i++)); do
    (./$library/client_bin $reliable_messages > /dev/null)  &
    pids+=($!)
    echo "Started client ($!)"
done

# Run simulation for 60 seconds
sleep $3

# Gracefully kill tshark
kill $tshark_pid

# Forcefully kill binaries
for pid in ${pids[@]}; do
	echo "Killing $pid"
	kill -9 $pid > /dev/null
done

# Process output data
echo "Processing packet dump for $library"
python3 network_usage_benchmark_output/process.py "network_usage_benchmark_output/$output_file_name"
