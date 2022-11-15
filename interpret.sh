FILE=code.ifjc22

make
cd out
./ifj22 > ../$FILE
cd ..
./ic22int -v $FILE