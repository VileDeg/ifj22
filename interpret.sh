FILE=code.ifjc22

make
cd out
./ifj22 -src > ../code.ifjc22
cd ..
./ic22int code.ifjc22