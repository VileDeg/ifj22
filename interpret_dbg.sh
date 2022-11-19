FILE=code.ifjc22

make
cd out
./ifj22 -src > ../code.ifjc22
cd ..
./ic22int -v code.ifjc22
echo $?