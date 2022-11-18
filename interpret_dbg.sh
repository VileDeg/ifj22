FILE=code.ifjc22

make
cd out
./ifj22 -src > ../$FILE
cd ..
./ic22int -v $FILE