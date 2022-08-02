TESTING FRAMEWORK FOR IFJ LANGUAGES

Autor: Kamil Michl

Upozornění: Autor tohoto repozitáře NERUČÍ za správnost a korektnost vytvořených scriptů ani testů.
Upozornění: Funkčnost scriptu je ověřena na servru merlin, na jiných strojích se může postup spuštění a instalace lišit.

Základní myšlenkou testování je porovnání výsledku IFJ překladače a interpretru s výsledkami stejného programu v nativním jazyce.

Soubory:
README.txt      - Tento soubor.
testsuite.py    - Jádro testovacího programu.
ifj<year>.<ext> - soubor obsahující funkce pro umožnění interpretace IFJ jazyka interpretem nativního jazyka.
ic<year>int     - referenční interpret ze stránek předmětu IFJ.
log             - Vytvořený testovacím scriptem. Obsahuje detailnější záznamy o proběhnlých testech.
                  Varování: Tento soubor se smaže při každém spuštění testovacího scriptu.

Adresáře:
tests           - Zde se nachází zdrojové soubory jednotlivých testů.
outputs         - Vytvořený testovacím scriptem. Obsahuje testy přeložené do mezikódu, které selhaly.
                  Varování: Tento adresář se smaže při každém spuštění testovacího scriptu.

Použití:
Testy spustíte vykonáním příkazu "python ./testsuite.py".
Testovací script obsahuje celou řadu parametrů, kterými lze upravit/nastavit chování scriptu.
Doporučuji seznámit se s těmito parametry pomocí spuštění scriptu s parametrem "--help".
Jednotlivé testy jsou rozřazeny do složek podle typu.
Pro supštění pouze konkrétních typů testů použijte parameter "--select" případně "--select-file".
Dalšími užitečnými parametry jsou například "--mode-compile-only nebo --log-file".

Přidávání testů:
Nové testy lze přidávat jednoduše vytvořením testovacího programu v novém souboru
v adresáři tests (nebo jeho podadresářích). Tento nový testovací soubor musí mít
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
	Pokud seznam obsahuje nulu, následuje po úspěšném překladu interpretace.
	Pokud tento řádek není přítomen, je nula jediný validní návratový kód.
"//interpret <interpret-codes>":
	Seznam validních návratových kódů interpretu pro tento test.
	Jednotlivé číselné kódy jsou odděleny mezerou.
	Pokud seznam obsahuje nulu, následuje po úspěšné interpretaci IFJ interpretrem porovnání s výsledkem nativního interpretu.
	Pokud tento řádek není přítomen, je nula jediný validní návratový kód.
"//no native":
	Pokud je tento řádek přítomen, neprovádí se porovnání výsledků interpretace s výsledky nativního interpretu.
	Toto je vhodné pro situace, které IFJ jazyk zpracovává jinak než nativní jazyk.
"//extensions+ <extensions>":
	Seznam rozšíření, které je nutné implementovat pro korektní průběh daného testu.
	Rozšíření jsou uváděna v textová podobě (stejné jako v zadání projektu) a jsou oddělena mezerou.
	Pokud tento řádek není přítomen, není nutné mít implementováno žádné rozšíření pro korektní výsledek testu.
"//extensions- <extensions>":
	Seznam rozšíření, které nesmí být implementovány pro korektní průběh daného testu.
	Rozšíření jsou uváděna v textová podobě (stejné jako v zadání projektu) a jsou oddělena mezerou.
	Pokud tento řádek není přítomen, není zakázana implementace žádného rozšíření pro korektní výsledek testu.
"//input <input-file>":
	Cesta k souboru (relativní k testu), který obsahuje vstup pro testovací program.
	Tento řádek může být přítomný vícekrát a pro každý je vytvořen samostatný testovací scénář.
