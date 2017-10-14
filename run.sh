SMD=./smd

$SMD server &> server.log
SMD_SERVER_PID=$!

$SMD client &> client.log
SMD_CLIENT_PID=$!
