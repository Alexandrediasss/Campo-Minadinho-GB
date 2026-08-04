#include <gb/gb.h>
#include "sprites.h"

// 1. Sua struct otimizada para o Game Boy (8-bits)
typedef struct {
    UINT8 hasBomb;
    UINT8 isAppearing;
    UINT8 flag;
    UINT8 neighbor;
} Block;

// 2. Variáveis globais do jogo
Block minefield[10][10];
UINT8 isShowingCount = 0;
UINT8 isGaming = 0;
UINT8 flagCount = 0;

UINT8 cursor_x = 48;
UINT8 cursor_y = 48;

// 3. Sua função adaptada para usar INT8 (para não dar erro no r-1)
UINT8 bombCounter(INT8 r, INT8 c) {
    UINT8 count = 0;

    if (r-1 >= 0 && c-1 >= 0 && minefield[r-1][c-1].hasBomb == 1) count++;
    if (r-1 >= 0 && minefield[r-1][c].hasBomb == 1) count++;
    if (r-1 >= 0 && c+1 < 10 && minefield[r-1][c+1].hasBomb == 1) count++;
    if (c-1 >= 0 && minefield[r][c-1].hasBomb == 1) count++;
    if (c+1 < 10 && minefield[r][c+1].hasBomb == 1) count++;
    if (r+1 < 10 && c-1 >= 0 && minefield[r+1][c-1].hasBomb == 1) count++;
    if (r+1 < 10 && minefield[r+1][c].hasBomb == 1) count++;
    if (r+1 < 10 && c+1 < 10 && minefield[r+1][c+1].hasBomb == 1) count++;

    return count;
}

void main(void)
{
    // 1. Carrega os gráficos na VRAM dando um ID para cada um
    set_bkg_data(0, 1, blank_bg);     // ID 0
    set_bkg_data(1, 1, closed_block); // ID 1
    set_bkg_data(2, 1, happy_face);   // ID 2
    set_bkg_data(3, 1, empty_block);  // ID 3
    set_bkg_data(12, 1, flag);        // ID 12

    // Pinta a tela INTEIRA com o fundo liso (ID 0)
    unsigned char full_map[360];
    for (UINT16 i = 0; i < 360; i++) full_map[i] = 0;
    set_bkg_tiles(0, 0, 20, 18, full_map);

    // Desenha a Happy Face (ID 2) no topo
    unsigned char face_map[] = {2};
    set_bkg_tiles(9, 1, 1, 1, face_map);

    // Desenha o Tabuleiro 10x10 (ID 1) no centro da tela
    unsigned char board_map[100];
    for (UINT16 i = 0; i < 100; i++) board_map[i] = 1;
    set_bkg_tiles(5, 4, 10, 10, board_map);

    // Configura o cursor
    set_sprite_data(0, 1, cursor);
    set_sprite_tile(0, 0);
    move_sprite(0, cursor_x, cursor_y);

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    while (1)
    {
        UINT8 input = joypad();

        // Movimento do cursor
        if (input & J_UP && cursor_y > 48) { cursor_y -= 8; delay(150); }
        if (input & J_DOWN && cursor_y < 120) { cursor_y += 8; delay(150); }
        if (input & J_LEFT && cursor_x > 48) { cursor_x -= 8; delay(150); }
        if (input & J_RIGHT && cursor_x < 120) { cursor_x += 8; delay(150); }

        move_sprite(0, cursor_x, cursor_y);

        // AÇÃO 1: REVELAR BLOCO (Botão X no teclado)
        if (input & J_A)
        {
            UINT8 col = (cursor_x - 48) / 8;
            UINT8 row = (cursor_y - 48) / 8;

            if (minefield[row][col].isAppearing == 0 && minefield[row][col].flag == 0)
            {
                minefield[row][col].isAppearing = 1;
                isShowingCount++;
                
                // Converte a posição do cursor para a posição dos tiles na tela
                UINT8 tile_x = col + 5;
                UINT8 tile_y = row + 4;
                
                // Carimba o BLOCO VAZIO (ID 3)
                unsigned char new_tile[1] = {3};
                set_bkg_tiles(tile_x, tile_y, 1, 1, new_tile);
                
                delay(200); 
            }
        }

        // AÇÃO 2: COLOCAR/TIRAR BANDEIRA (Botão Z no teclado)
        if (input & J_B)
        {
            UINT8 col = (cursor_x - 48) / 8;
            UINT8 row = (cursor_y - 48) / 8;

            if (minefield[row][col].isAppearing == 0)
            {
                UINT8 tile_x = col + 5;
                UINT8 tile_y = row + 4;
                unsigned char tile_update[1];

                if (minefield[row][col].flag == 1)
                {
                    minefield[row][col].flag = 0;
                    flagCount--;
                    // Tira a bandeira, carimba o BLOCO FECHADO de novo (ID 1)
                    tile_update[0] = 1; 
                    set_bkg_tiles(tile_x, tile_y, 1, 1, tile_update);
                }
                else
                {
                    minefield[row][col].flag = 1;
                    flagCount++;
                    // Coloca a BANDEIRA (ID 12)
                    tile_update[0] = 12; 
                    set_bkg_tiles(tile_x, tile_y, 1, 1, tile_update);
                }
                delay(200); 
            }
        }
        wait_vbl_done();
    }
}