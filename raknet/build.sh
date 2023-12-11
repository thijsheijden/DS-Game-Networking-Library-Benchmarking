echo "Building RakNet client and server"
cmake -S . -B build > /dev/null
cmake --build build > /dev/null
mv build/client client_bin
mv build/server server_bin
