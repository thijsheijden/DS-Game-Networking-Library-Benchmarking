# Library argument
library=$1
echo "Running network metrics benchmark for $library"

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
(./$library/server_bin > /dev/null) &
pids+=($!)
echo "Started server ($!)"

# Start two clients
(./$library/client_bin > /dev/null) &
pids+=($!)
echo "Started client 1 ($!)"
(./$library/client_bin > /dev/null) &
pids+=($!)
echo "Started client 2 ($!)"

# Run simulation for 60 seconds
sleep 60

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
