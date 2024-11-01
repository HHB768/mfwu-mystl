#!/bin/bash

line_cnt=$(cat ./lib/*.hpp ./test/*.hpp | wc -l)

today=$(date +%s)
target_date=$(date -d "2024-06-16" +%s)
days_diff=$(( (today - target_date) / 86400 ))

res=$(( line_cnt / days_diff ))

echo "Average line cnt since 24.04.21: $res"

