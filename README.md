# IFJ22
## Clone
To clone the repo type
```sh
git clone https://username@github.com/VileDeg/ifj22
```
where username is your github username.
## Правила к оформлению
- Всегда использовать int64_t вместо int, uint64_t вместо unsigned
- Информативные названия функций
## TODO
Scanner:
- Вернуть union
- Вернуть проверку на первый символ == '<'
- Пофиксить обработку комментов (некоторые тесты проходят хотя не должны)
- $5var -> ошибка + проблемы с $
- проблемы с эскейп секвенциями(исправить только в .drawio)
- убрать нумерацию строк в лексикальной ошибке 
- Empty file -> syntax error? 
- Документация(в коде и в .pdf)
Code generator:
- ? Выписывать код только в самом конце когда никакой ошибки нет
## Source files
- debug.h    - declares functions for testing/debugging.
- scanner.h  - scanner.
- string_t.h - dynamic string class.
- main.c     - program entry point.
- ```...```
## Build and run
```sh
make ; out/ifj22
```
or
```sh
make clean ; make ; out/ifj22
```
