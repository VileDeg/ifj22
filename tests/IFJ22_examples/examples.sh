#/bin/bash

# IFJ22 - Priklady pro studenty pro dokresleni zadani (spusteni konfigurovano pro server Merlin)

COMPILER="php8.1 "  # zde muze byt vas prekladac IFJ22 (napr. ./ifj22)
HELPER="ifj22.php"
INTERPRETER="php8.1 ${HELPER} "  # zde muze byt interpret IFJcode22 (./ic22int)

for i in *.php; do

	if [[ $i == $HELPER ]]; then
		continue
	fi

	IN=`echo $i | sed 's/\(.*\)\.php/\1.in/g'`
	OUT=`echo $i | sed 's/\(.*\)\.php/\1.out/g'`
	PRG=$i

	# $COMPILER $i $HELPER > $PRG   # nekontroluji se chyby prekladu (pro chybove testy nutno rozsirit)
	# RETURNED_COMPILER=$?

	echo -n "DIFF: $INTERPRETER $PRG and $OUT: "
	cat $IN | $INTERPRETER $PRG 2> >(sed $'s,.*,\e[31m&\e[m,'>&2) | diff - $OUT > /dev/null 2> /dev/null
	DIFFOK=$?
	if [ "x$DIFFOK" == "x0" ]; then
		echo "OK"
	else
		echo "DIFFERENCE to $OUT"
		cat $IN | $INTERPRETER $PRG 2> >(sed $'s,.*,\e[31m&\e[m,'>&2) | diff - $OUT 
	fi
        echo
done;

# rm *.tmp

exit 0
