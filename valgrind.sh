make
cd out
valgrind --leak-check=full --show-leak-kinds=all ./ifj22 -src
