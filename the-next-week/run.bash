cd build
cmake ..
make
./main > ../img/temp.ppm
cd ../utils
./ppm2bmp ../img/temp.ppm ../img/bmp/sample01.bmp