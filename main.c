#include <gb/gb.h>
#include <rand.h> 
#include "sprites.h"

// Struct otimizada para 8-bits
typedef struct {
    UINT8 hasBomb;
    UINT8 isAppearing;
    UINT8 flag;
    UINT8 neighbor;
} Block;

Block minefield[10][10];
UINT8 isShowingCount = 0;
UINT8 isGaming = 0;
UINT8 flagCount = 0;

UINT8 cursor_x = 48;
UINT8 cursor_y = 48;

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

// Sua função de gerar o mapa adaptada
void startGame(void) {
    isShowingCount = 0;
    isGaming = 0;
    flagCount = 0;

    for(UINT8 i = 0; i < 10; i++){
        for(UINT8 j = 0; j < 10; j++){
            minefield[i][j].hasBomb = 0;
            minefield[i][j].isAppearing = 0;
            minefield[i][j].flag = 0;
            minefield[i][j].neighbor = 0;
        }
    }

    UINT8 bomb_count = 0;
    while(bomb_count < 10){
        UINT8 num1 = rand() % 10;
        UINT8 num2 = rand() % 10;

        if(minefield[num1][num2].hasBomb == 0){
            minefield[num1][num2].hasBomb = 1;
            bomb_count++;
        }
    }
}

void resetGame(void) {
    startGame();

    // Pinta a tela INTEIRA com o fundo liso
    unsigned char full_map[360];
    for (UINT16 i = 0; i < 360; i++) full_map[i] = 0;
    set_bkg_tiles(0, 0, 20, 18, full_map);

    // Desenha a Happy Face
    unsigned char face_map[] = {2};
    set_bkg_tiles(9, 1, 1, 1, face_map);

    // Desenha o Tabuleiro
    unsigned char board_map[100];
    for (UINT16 i = 0; i < 100; i++) board_map[i] = 1;
    set_bkg_tiles(5, 4, 10, 10, board_map);

    cursor_x = 48;
    cursor_y = 48;
    move_sprite(0, cursor_x, cursor_y);
}

void main(void)
{
    // PASSO 1: Carrega TODOS os gráficos na VRAM
    set_bkg_data(0, 1, blank_bg);     // ID 0
    set_bkg_data(1, 1, closed_block); // ID 1
    set_bkg_data(2, 1, happy_face);   // ID 2
    set_bkg_data(3, 1, empty_block);  // ID 3
    set_bkg_data(4, 1, block_1);      // ID 4
    set_bkg_data(5, 1, block_2);      // ID 5
    set_bkg_data(6, 1, block_3);      // ID 6
    set_bkg_data(7, 1, block_4);      // ID 7
    set_bkg_data(8, 1, block_5);      // ID 8
    set_bkg_data(9, 1, block_6);      // ID 9
    set_bkg_data(10, 1, block_7);     // ID 10
    set_bkg_data(11, 1, block_8);     // ID 11
    set_bkg_data(12, 1, flag);        // ID 12
    set_bkg_data(13, 1, bomb);        // ID 13
    set_bkg_data(14, 1, bomb_explosion); // ID 14
    set_bkg_data(15, 1, wrong_flag);  // ID 15

    set_sprite_data(0, 1, cursor);
    set_sprite_tile(0, 0);

    // PASSO 2: Inicia as variáveis e desenha o mapa
    resetGame();

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    while (1)
    {
        UINT8 input = joypad();

        // Verifica se o jogo acabou (Vitória ou Derrota)
        if (isGaming == 1)
        {
            if (input & J_START || input & J_A)
            {
                resetGame();
                delay(200);
            }
            wait_vbl_done();
            continue;
        }

        // Condição de Vitória (90 blocos abertos + 10 bandeiras colocadas)
        if (flagCount == 10 && isShowingCount == 90)
        {
            isGaming = 1; // Para o jogo e permite reiniciar
            continue;
        }

        // Movimento
        if (input & J_UP && cursor_y > 48) { cursor_y -= 8; delay(150); }
        if (input & J_DOWN && cursor_y < 120) { cursor_y += 8; delay(150); }
        if (input & J_LEFT && cursor_x > 48) { cursor_x -= 8; delay(150); }
        if (input & J_RIGHT && cursor_x < 120) { cursor_x += 8; delay(150); }

        move_sprite(0, cursor_x, cursor_y);

        // PASSO 3: Lógica de Revelação Completa (Botão X)
        if (input & J_A)
        {
            UINT8 col = (cursor_x - 48) / 8;
            UINT8 row = (cursor_y - 48) / 8;

            if (minefield[row][col].isAppearing == 0 && minefield[row][col].flag == 0)
            {
                minefield[row][col].isAppearing = 1;
                isShowingCount++;
                
                UINT8 tile_x = col + 5;
                UINT8 tile_y = row + 4;
                unsigned char new_tile[1];

                if (minefield[row][col].hasBomb == 1)
                {
                    // Explodiu! 
                    isGaming = 1;
                    new_tile[0] = 14; // Carimba a bomba explodindo
                }
                else
                {
                    // Descobre quantos vizinhos tem bomba
                    minefield[row][col].neighbor = bombCounter(row, col);
                    
                    // Se vizinho for 0, desenha o ID 3 (empty_block).
                    // Se vizinho for 1, desenha o ID 4 (block_1), e assim por diante.
                    new_tile[0] = 3 + minefield[row][col].neighbor;
                }
                
                set_bkg_tiles(tile_x, tile_y, 1, 1, new_tile);
                delay(200); 
            }
        }

        // AÇÃO 2: COLOCAR/TIRAR BANDEIRA (Botão Z)
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
                    tile_update[0] = 1; // Volta pro bloco fechado
                    set_bkg_tiles(tile_x, tile_y, 1, 1, tile_update);
                }
                else
                {
                    minefield[row][col].flag = 1;
                    flagCount++;
                    tile_update[0] = 12; // Põe a bandeira
                    set_bkg_tiles(tile_x, tile_y, 1, 1, tile_update);
                }
                delay(200); 
            }
        }
        wait_vbl_done();
    }
}