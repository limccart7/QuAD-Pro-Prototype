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
rm -rf lg

cd /src/utils
gcc spi_loopback_test.c -o spi_loopback_test -llgpio
