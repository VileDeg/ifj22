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
- TYPE_NIL = void?
- убрать нумерацию строк в лексикальной ошибке 
- Empty file -> syntax error? 
- Документация(в коде и в .pdf)
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
