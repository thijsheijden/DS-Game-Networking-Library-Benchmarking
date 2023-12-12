echo "Building yojimbo client and server"
make yojimbo > /dev/null
make server > /dev/null
make client > /dev/null
mv bin/client client_bin
mv bin/server server_bin