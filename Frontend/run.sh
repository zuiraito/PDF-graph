#!/bin/bash

# Set the port number
PORT=8000


# Open Firefox with the local server URL
sleep 1 && firefox "http://localhost:$PORT" &

# Start Python HTTP server in the background
python3 -m http.server $PORT
