cd ./src/test
wget https://googletest.googlecode.com/files/gtest-1.7.0.zip
unzip gtest-1.7.0.zip
cd gtest-1.7.0
mkdir build
cd build
cmake ..
make
cd ../../
mkdir build
cd build
cmake ..
make
pwd
./popo_test
