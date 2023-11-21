FILE=../code.ifjc22
INT=./ic22int

make
./ifj22 -src > $FILE
$INT -v $FILE
echo -e "\n$?"