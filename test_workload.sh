#!/usr/bin/env bash

repeat=100

sudo sysctl net.ipv4.tcp_tw_reuse=1
sudo sysctl net.ipv4.tcp_tw_recycle=1

kill $(ps -ef | grep run_pt_server | awk '{print $2}')

sleep 20

for (( i=6656+512; i<=8192; i=i+512))
do
./build/run_pt_server ${i} >> ./output/pt_thread_2_${i}_${repeat}.csv &
sleep 2
for (( j=1; j<=5; j=j+1))
do
./build/run_client ${i} ${repeat}
sleep 5
done
kill $(ps -ef | grep run_pt_server | awk '{print $2}')
sleep 20
done

sudo sysctl net.ipv4.tcp_tw_reuse=0
sudo sysctl net.ipv4.tcp_tw_recycle=0
