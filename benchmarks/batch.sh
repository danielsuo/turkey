#!/usr/bin/env bash
# sudo python run.py blackscholes 1
# sleep 120
./bin/clean
sudo python run.py blackscholes 32
sleep 120
./bin/clean

sudo python run.py bodytrack 1
sleep 120
./bin/clean
sudo python run.py bodytrack 32
sleep 120
./bin/clean

sudo python run.py canneal 1
sleep 120
./bin/clean
sudo python run.py canneal 32
sleep 120
./bin/clean

sudo python run.py dedup 1
sleep 120
./bin/clean
sudo python run.py dedup 32
sleep 120
./bin/clean

sudo python run.py facesim 1
sleep 120
./bin/clean
sudo python run.py facesim 32
sleep 120
./bin/clean

sudo python run.py ferret 1
sleep 120
./bin/clean
sudo python run.py ferret 32
sleep 120
./bin/clean

sudo python run.py fluidanimate 1
sleep 120
./bin/clean
sudo python run.py fluidanimate 32
sleep 120
./bin/clean

sudo python run.py freqmine 1
sleep 120
./bin/clean
sudo python run.py freqmine 32
sleep 120
./bin/clean

sudo python run.py raytrace 1
sleep 120
./bin/clean
sudo python run.py raytrace 32
sleep 120
./bin/clean

sudo python run.py streamcluster 1
sleep 120
./bin/clean
sudo python run.py streamcluster 32
sleep 120
./bin/clean

sudo python run.py swaptions 1
sleep 120
./bin/clean
sudo python run.py swaptions 32
sleep 120
./bin/clean

sudo python run.py vips 1
sleep 120
./bin/clean
sudo python run.py vips 32
sleep 120
./bin/clean

sudo python run.py x264 1
sleep 120
./bin/clean
sudo python run.py x264 32
sleep 120
./bin/clean
