#!/bin/bash

# Chama o compilador GBDK e gera o arquivo do jogo (.gb)
../gbdk/bin/lcc -Wa-l -Wl-m -Wl-j -o campominadinho.gb main.c

echo "Compilação finalizada!"