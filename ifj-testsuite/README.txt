IMPORTANT: THE TESTSUITE IS NOT YET COMPLETE, ONLY COMPILER ERROR CODES ARE TESTED NOW.
           PLEASE USED THE '-mc' PARAMETER TO ENABLE COMPILER ONLY MODE UNTIL THE REST OF THE TESTSUITE IS DONE.

TESTING FRAMEWORK FOR IFJ LANGUAGES

Autor: Kamil Michl

Upozornění: Autor tohoto repozitáře NERUČÍ za správnost a korektnost vytvořených scriptů ani testů.
Upozornění: Funkčnost scriptu je ověřena na servru merlin, na jiných strojích se může postup spuštění a instalace lišit.

Soubory:
README.txt      - Tento soubor.
testsuite.py    - Jádro testovacího programu.
ifj<year>.<ext> - soubor obsahující funkce pro umožnění interpretace IFJ jazyka interpretem nativního jazyka. (TODO)
ic<year>int     - referenční interpret ze stránek předmětu IFJ. (TODO)
log             - Vytvořený testovacím scriptem. Obsahuje detailnější záznamy o proběhlých testech.
                  Varování: Tento soubor se smaže při každém spuštění testovacího scriptu.

Adresáře:
tests           - Zde se nachází zdrojové soubory jednotlivých testů.
outputs         - Vytvořený testovacím scriptem. Obsahuje testy přeložené do mezikódu.
                  Varování: Tento adresář se smaže při každém spuštění testovacího scriptu.

Použití:
Testy spustíte vykonáním příkazu "python3 ./testsuite.py" v adresáří, kde se tento soubor nachází.
Testovací script obsahuje celou řadu parametrů, kterými lze upravit/nastavit chování scriptu.
Doporučuji seznámit se s těmito parametry pomocí spuštění scriptu s parametrem "--help".
Jednotlivé testy jsou rozřazeny do složek podle typu v adresáři "tests".

Argumenty scriptu:
  -h, --help            show this help message and exit
  --mode-compile-only, -mc
                        compile only mode runs only compiler for each test (no interpretation and no output checking)
  --mode-interpret-only, -mi
                        interpret only mode runs only compiler and interpret for each test (no output checking)
  --mode-all, -ma       all mode runs compiler and interpret for each test and then checks outputs with native language interpret. This option is default
  --select SELECT, -s SELECT
                        single test file or test directory that should be run (relative to main tests folder). example: "lex/symbols". default: All tests in main tests directory are run
  --select-file SELECT_FILE, -sf SELECT_FILE
                        path to a file that specifies multiple test files or tests directories to be run (each on separate line).
  --quiet, -q           disable prints to standart output except final results and logs if '--log-output' is used
  --compiler COMPILER, -c COMPILER
                        path to the IFJ22 language compiler (the IFJ project executable). default: ./ifj22
  --extensions EXTENSIONS, -e EXTENSIONS
                        list of implemented extensions. example: "STRNUM,BOOLTHEN". default: No extension implemented. options: STRNUM, BOOLTHEN, CYCLESFUNEXP, GLOBAL
  --log-file LOG_FILE, -lf LOG_FILE
                        path to the log file created by the testsuite (if file already exists, it will be deleted). default: ./log
  --log-output, -lo     print logs to the standard output instead of a file
  --log-none, -ln       do not print logs to output or a file
  --log-success-output, -ls
                        log interpret output from successfull tests
  --save-ifjcode-all, -a
                        stores compiler results (ifjcode files) for all tests (not only failed ones)
  --timeout-native-interpret TIMEOUT_NATIVE_INTERPRET, -tn TIMEOUT_NATIVE_INTERPRET
                        specify maximum timeout for each run of native interpret in seconds (required to detect infinite run errors). defult = 5
  --timeout-ifjcode-interpret TIMEOUT_IFJCODE_INTERPRET, -ti TIMEOUT_IFJCODE_INTERPRET
                        specify maximum timeout for each run of ifjcode interpret in seconds (required to detect infinite run errors). defult = 5
  --timeout-compiler TIMEOUT_COMPILER, -tc TIMEOUT_COMPILER
                        specify maximum timeout for each run of ifj22 compiler in seconds (required to detect infinite run errors). defult = 5
  --output-folder OUTPUT_FOLDER, -o OUTPUT_FOLDER
                        path to the folder where compiler output (IFJ22code language programs) is stored for every test that fails on interpretation or checking (if folder already exists, it will be
                        deleted). default: ./outputs
  --native-interpret NATIVE_INTERPRET
                        command to execute native interpret for output checking. default: php8.1
  --ifjcode-interpret IFJCODE_INTERPRET
                        command to execute IFJ22code interpret for compiler output interpretation. default: ./ic22int
  --native-include-file NATIVE_INCLUDE_FILE
                        path to the file that is required to be included in native language programs to execute ifj language. default: ./ifj22.php
  --tmp-dir TMP_DIR     path to a temp directory that will be created to store temp files for tests. default: ./tmp

Přidávání testů:
Nové testy lze přidávat jednoduše vytvořením nového souboru, obsahujícího testovací program,
v adresáři "tests" (nebo jeho podadresářích). Tento nový testovací soubor musí mít
koncovku nativního jazyka a na začátku obsahovat testovací hlavičku, která obsahuje informace
o konkrétních testovacích případech (návratové kódy, vstupy, atd.).
Přesný formát hlavičky bude popsán níže. Pokud chcete poskytnout testy i ostatním
studentům, vytvářejte testy v samostatných větvích gitu a poté vytvořte merge/pull request,
abych mohl testy integrovat do master větve nebo mě kontaktuje přes discord "kam29#4080"

hlavička testů:
Každý testovací soubor musí na svém začátku obsahovat hlavičku ukončenou řádkem "//"
(i pokud je hlavička prázdná). Hlavička může obsahovat následující řádky:
"//compiler <compiler-codes>":
	Seznam validních návratových kódů překladače pro tento test.
	Jednotlivé číselné kódy jsou odděleny mezerou.
	Pokud seznam obsahuje nulu a je pro test definován návratový kód interpretru, následuje po úspěšném překladu interpretace.
	Pokud tento řádek není přítomen, je nula jediný validní návratový kód.
"//interpret <interpret-codes>":
	Seznam validních návratových kódů interpretu pro tento test.
	Jednotlivé číselné kódy jsou odděleny mezerou.
	Pokud seznam obsahuje nulu a test má být spouštěn a nativním interpretrem, následuje po úspěšné interpretaci IFJ interpretrem porovnání s výsledkem nativního interpretu.
	Pokud tento řádek není přítomen, je nula jediný validní návratový kód.
"//native":
	Pokud je tento řádek přítomen, provádí se porovnání výsledků interpretace s výsledky nativního interpretu.
	Toto je vhodné pro kód, který IFJ jazyk zpracovává totožným způsobem jako nativní jazyk.
"//extensions+ <extensions>":
	Seznam rozšíření, které je nutné implementovat pro korektní průběh daného testu.
	Rozšíření jsou uváděna v textové podobě (stejné jako v zadání projektu) a jsou oddělena mezerou.
	Pokud tento řádek není přítomen, není nutné mít implementováno žádné rozšíření pro spuštění deného testu.
"//extensions- <extensions>":
	Seznam rozšíření, které nesmí být implementovány pro korektní průběh daného testu.
	Rozšíření jsou uváděna v textová podobě (stejné jako v zadání projektu) a jsou oddělena mezerou.
	Pokud tento řádek není přítomen, není zakázana implementace žádného rozšíření pro spuštění daného testu.
"//input <input-file>":
	Cesta k souboru (relativní k testu), který obsahuje vstup pro testovací program.
	Pokud tento řádek není přítomen, interpret (IFJ i nativní) nedostane na vstup žádná data.
"//output <output-file>":
	Cesta k souboru (relativní k testu), který obsahuje referenční výstup pro testovací program.
	Pokud tento řádek není přítomen, výstup IFJ interpretru se nekontroluje (s výjimkou případné kontroly s nativním interpretrem).
	Pokud tento řádek je přítomen, proběhne bezprostředně po úspěšném běhu IFJ interpretru kontrola jeho výstupu s referenčním výstupem.
"//<comment>":
	Jakýkoliv uživatelský text, pro účeli vlastností testu je ignorován a pro spuštění program je odstraněn společně se zbytkem hlavičky.