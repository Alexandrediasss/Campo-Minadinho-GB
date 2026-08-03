#include <gb/gb.h>
#include <stdio.h>

void main(void) {
    printf(" \n");
    printf(" Ola, Game Boy!\n\n");
    printf(" Compilador pronto!\n");
    
    // Loop infinito para o jogo não fechar imediatamente
    while(1) {
        wait_vbl_done(); // Espera a tela atualizar (boa prática no Game Boy)
    }
}