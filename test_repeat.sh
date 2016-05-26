#!/usr/bin/env bash

sudo sysctl net.ipv4.tcp_tw_reuse=1
sudo sysctl net.ipv4.tcp_tw_recycle=1

kill $(ps -ef | grep run_pt_server | awk '{print $2}')

sleep 10

for (( i=100; i<=1000; i=i+100 ))
do
./build/run_pt_server 2048 >> ./output/pt_wl_4_2048_${i}.csv &
sleep 5
for (( j=1; j<=5; j=j+1 ))
do
./build/run_client 2048 ${i}
sleep 2
done
kill $(ps -ef | grep run_pt_server | awk '{print $2}')
sleep 10
done

sudo sysctl net.ipv4.tcp_tw_reuse=0
sudo sysctl net.ipv4.tcp_tw_recycle=0
