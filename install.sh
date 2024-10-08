# install script for Project-OWL quad pro
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install unzip
sudo apt-get install libhiredis-dev

sudo apt-get update
sudo apt-get install redis-server
sudo apt-get install -y cmake 

wget http://abyz.me.uk/lg/lg.zip
unzip lg.zip
cd lg
make
sudo make install
cd ..
sudo rm -rf lg
sudo rm -rf lg.zip
#!/bin/bash


# making executable for spi loopback test
# pwd /QuAD-Pro-Prototype/src/utils
cd src/utils
gcc spi_loopback_test.c -o spi_loopback_test -llgpio
cd messageBroker
mkdir build
cd build
cmake ..
make 

cd ..
cd ..
cd ..
# pwd /Quad-Pro-Prototype/src
# Cmake stuff to ensure cmakecache is local to user, and executable can be run

cd LoRa
mkdir build
cd build
cmake ..
make 

cd .. 
cd ..
#pwd is /QuAD-Pro-Prototyple/src
cd CDP
mkdir build
cd build
cmake ..
make 
# cmake stuff for receive example
cd .. 
cd ..
cd utils
cd rx
mkdir build
cd build
cmake ..
make 
# cmake stuff for transmit example
cd .. 
cd ..
cd tx
mkdir build
cd build
cmake ..
make 
#no cmake for web-interface as it is written in python
