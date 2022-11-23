FILE=../code.ifjc22
INT=./../ic22int

make
cd out
./ifj22 -src > $FILE
$INT $FILE
echo -e "\n$?"