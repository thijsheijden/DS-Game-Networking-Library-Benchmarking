# Libraries to test, directory should have the same name
libraries=("raknet")

for library in ${libraries[@]}; do
	# Tracked started background processes
	pids=()

	cd $library

	# Create build dir
	cmake -S . -B build > /dev/null

	# Build client and server binaries
	echo "Building $library client and server"
	cmake --build build > /dev/null
	
	# Start logging network traffic using tshark
	output_file_name="network_metrics_"$library"_$(date +"%s").csv"
	echo "Logging network metrics to $output_file_name"
	tshark -i lo0 -f "port 60000" -T fields -e frame.time_relative -e frame.number -e frame.len -E header=y -E separator=, > "../network_usage_benchmark_output/$output_file_name" &
	pids+=($!)
	echo "Started tshark ($!) logging of packets to and from port 60,000"

	# Start the server in background process
	(./build/server > /dev/null) &
	pids+=($!)
	echo "Started server ($!)"
	
	# Start two clients
	(./build/client > /dev/null) &
	pids+=($!)
	echo "Started client 1 ($!)"
	(./build/client > /dev/null) &
	pids+=($!)
	echo "Started client 2 ($!)"

	# Run simulation for 60 seconds
	sleep 60

	# Gracefully kill all started jobs
	for pid in ${pids[@]}; do
		echo "Killing $pid"
		kill -2 $pid > /dev/null
	done

	# Process output data
	echo "Processing packet dump for $library"
	cd ../network_usage_benchmark_output
	python3 process.py "$output_file_name"
done
