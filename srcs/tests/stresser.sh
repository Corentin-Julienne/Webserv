#!/bin/bash
# This script has been made by mpeharpr in the context of the 42 project "webserv".
# It sends a specified number of requests to a specified URL and prints a summary.
# Please note that this script is not intended to be used for any other purpose than

# Check arguments
if [ "$#" -lt 2 ]; then
  echo -e "\e[31mUsage:\e[0m $0 \e[4mURL\e[0m \e[4mnumber_of_requests\e[0m [\e[4mnumber_of_simultaneous_connections\e[0m]"
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
        ((num_fail++))
      fi
    done
    pids=()
  fi
  echo -ne "\r\e[36m$i / $num_requests\e[0m"
done

# Wait for remaining background tasks
for pid in ${pids[*]}; do
  if wait $pid; then
    ((num_success++))
  else
    ((num_fail++))
  fi
done

# Calculate duration of execution
end_time=$(date +%s)
duration=$((end_time - start_time))

# Summary
echo -e "\nNumber of successful requests: \e[32m$num_success\e[0m"
echo -e "Number of failed requests: \e[31m$num_fail\e[0m"
echo -e "Duration of execution: \e[36m$duration seconds\e[0m"
