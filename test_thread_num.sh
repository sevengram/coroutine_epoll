#!/usr/bin/env bash

sudo sysctl net.ipv4.tcp_tw_reuse=1
sudo sysctl net.ipv4.tcp_tw_recycle=1

client_thread=4096

client_repeat=100

kill $(ps -ef | grep run_co_server | awk '{print $2}')

sleep 20

for (( i=1; i<=128; i=i*2))
do
./build/run_co_server ${i} ${client_thread} > ./output/thread_${i}_4096_100.csv &
sleep 2
for (( j=1; j<=10; j=j+1))
do
./build/run_client ${client_thread} ${client_repeat}
sleep 1
done
sleep 4
kill $(ps -ef | grep run_co_server | awk '{print $2}')
sleep 20
done

sudo sysctl net.ipv4.tcp_tw_reuse=0
sudo sysctl net.ipv4.tcp_tw_recycle=0
