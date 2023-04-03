#!/bin/bash
# This script has been made by mpeharpr in the context of the 42 project "webserv".
# It sends a specified number of requests to a specified URL and prints a summary.
# Please note that this script is not intended to be used for any other purpose than

# Check arguments
if [ "$#" -lt 2 ]; then
  echo -e "Usage: $0 URL number_of_requests [number_of_simultaneous_connections]"
  exit 1
fi

# URL and number of requests
url=$1
num_requests=$2

# Number of simultaneous connections
if [ "$#" -ge 3 ]; then
  num_connections=$3
else
  num_connections=1
fi

# Variables for summary
num_success=0
num_fail=0
start_time=$(date +%s)

# Send requests with curl
for (( i=1; i<=$num_requests; i++ )); do
  curl -sSf "$url" >/dev/null 2>&1 &
  pids+=($!)
  if [ $((i%num_connections)) -eq 0 ]; then
    for pid in ${pids[*]}; do
      if wait $pid; then
        ((num_success++))
      else
        echo -e "\nRequest failed now: $url"
        ((num_fail++))
      fi
    done
    pids=()
  fi
  echo -ne "\r$i / $num_requests"
done

# Wait for remaining background tasks
for pid in ${pids[*]}; do
  if wait $pid; then
    ((num_success++))
  else
    echo -e "\nRequest failed: $url"
    ((num_fail++))
  fi
done

# Calculate duration of execution
end_time=$(date +%s)
duration=$((end_time - start_time))

# Summary
echo -e "\nNumber of successful requests: $num_success"
echo -e "Number of failed requests: $num_fail"
echo -e "Duration of execution: $duration seconds"
