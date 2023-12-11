echo "Building Enet client"
cd EnetClient
cmake -S . -B build > /dev/null
cmake --build build > /dev/null
mv build/EnetClient ../client_bin

echo "Building Enet server"
cd ../EnetServer
cmake -S . -B build > /dev/null
cmake --build build > /dev/null
mv build/EnetServer ../server_bin
cd ..
