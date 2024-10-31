#!/bin/bash

line_cnt=$(cat ./lib/*.hpp ./test/*.hpp | wc -l)

# Get today's date
today=$(date +%s)

# Get the target date (April 21, 2024)
target_date=$(date -d "2024-06-16" +%s)

# Calculate the difference in days
days_diff=$(( (today - target_date) / 86400 ))

res=$(( line_cnt / days_diff ))

echo "Average line cnt since 24.04.21: $res"

