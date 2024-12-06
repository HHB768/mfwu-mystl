#!/bin/bash

line_cnt=$(cat ./lib/*.hpp ./test/*.hpp | wc -l)
# from_day="2024-05-11"
from_day="2024-06-16"
today=$(date +%s)
target_date=$(date -d $from_day +%s)
days_diff=$(( (today - target_date) / 86400 ))

res=$(( line_cnt / days_diff ))

echo "During these $days_diff days"
echo "Total line cnt is $line_cnt"
echo "Average line cnt since $from_day is $res"

