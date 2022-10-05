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
- Изменить 001 do 377
- Имплементировать грамматику
- Таблица символов в файле symtable.c/h
- Таблица для precedenční analýzy výrazů
- Встроенные функции
- Генерация кода
- Таблица LL-gramatiky
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
