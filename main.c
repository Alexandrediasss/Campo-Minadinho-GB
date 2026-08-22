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
UINT8 current_state = 0;
UINT8 menu_option = 0;

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

void drawEndScreen(UINT8 isVictory) {
    // Pinta a tela INTEIRA com o fundo liso
    unsigned char full_map[360];
    for (UINT16 i = 0; i < 360; i++) full_map[i] = 0;
    
    // Borda
    for (UINT8 i = 0; i < 20; i++) {
        full_map[i] = 1; // Top
        full_map[17 * 20 + i] = 1; // Bottom
    }
    for (UINT8 i = 0; i < 18; i++) {
        full_map[i * 20] = 1; // Left
        full_map[i * 20 + 19] = 1; // Right
    }
    set_bkg_tiles(0, 0, 20, 18, full_map);

    if (isVictory) {
        // PARABENS,
        unsigned char parabens_text[] = {26, 16, 27, 16, 28, 20, 24, 30, 35};
        set_bkg_tiles(5, 5, 9, 1, parabens_text);
        
        // VOCE VENCEU!
        unsigned char venceu_text[] = {33, 25, 17, 20, 0, 33, 20, 24, 17, 20, 32, 34};
        set_bkg_tiles(4, 7, 12, 1, venceu_text);
    } else {
        // GAME OVER
        unsigned char gameover_text[] = {29, 16, 23, 20, 0, 25, 33, 20, 27};
        set_bkg_tiles(5, 6, 9, 1, gameover_text);
    }

    // REINICIAR
    unsigned char reiniciar_text[] = {27, 20, 22, 24, 22, 17, 22, 16, 27};
    set_bkg_tiles(7, 12, 9, 1, reiniciar_text);

    // MENU
    unsigned char menu_text[] = {23, 20, 24, 32};
    set_bkg_tiles(7, 14, 4, 1, menu_text);

    // Configura o cursor na primeira opção
    menu_option = 0;
    cursor_x = 48;
    cursor_y = 112;
    move_sprite(0, cursor_x, cursor_y);
}

void drawStartScreen(void) {
    // Pinta a tela INTEIRA com o fundo liso
    unsigned char full_map[360];
    for (UINT16 i = 0; i < 360; i++) full_map[i] = 0;
    
    // Borda
    for (UINT8 i = 0; i < 20; i++) {
        full_map[i] = 1; // Top
        full_map[17 * 20 + i] = 1; // Bottom
    }
    for (UINT8 i = 0; i < 18; i++) {
        full_map[i * 20] = 1; // Left
        full_map[i * 20 + 19] = 1; // Right
    }
    set_bkg_tiles(0, 0, 20, 18, full_map);

    // CAMPO (IDs: C=17, A=16, M=23, P=26, O=25)
    unsigned char campo_text[] = {17, 16, 23, 26, 25};
    set_bkg_tiles(7, 5, 5, 1, campo_text);

    // MINADINHO (IDs: M=23, I=22, N=24, A=16, D=19, I=22, N=24, H=21, O=25)
    unsigned char minadinho_text[] = {23, 22, 24, 16, 19, 22, 24, 21, 25};
    set_bkg_tiles(5, 7, 9, 1, minadinho_text);

    // COMEÇAR (IDs: C=17, O=25, M=23, E=20, Ç=18, A=16, R=27)
    unsigned char comecar_text[] = {17, 25, 23, 20, 18, 16, 27};
    set_bkg_tiles(6, 12, 7, 1, comecar_text);

    // SAIR (IDs: S=30, A=16, I=22, R=27)
    unsigned char sair_text[] = {30, 16, 22, 27};
    set_bkg_tiles(8, 14, 4, 1, sair_text);

    menu_option = 0;
    cursor_x = 48; // tile x=5 (offset de sprite +8)
    cursor_y = 112;
    move_sprite(0, cursor_x, cursor_y); // Mostra o cursor
}

void revealZeros(void) {
    UINT8 changed = 1;
    INT8 r, c, dr, dc, nr, nc;
    while (changed) {
        changed = 0;
        for (r = 0; r < 10; r++) {
            for (c = 0; c < 10; c++) {
                if (minefield[r][c].isAppearing == 1 && minefield[r][c].hasBomb == 0 && minefield[r][c].neighbor == 0) {
                    for (dr = -1; dr <= 1; dr++) {
                        for (dc = -1; dc <= 1; dc++) {
                            if (dr == 0 && dc == 0) continue;
                            nr = r + dr;
                            nc = c + dc;
                            if (nr >= 0 && nr < 10 && nc >= 0 && nc < 10) {
                                if (minefield[nr][nc].isAppearing == 0 && minefield[nr][nc].flag == 0) {
                                    minefield[nr][nc].isAppearing = 1;
                                    isShowingCount++;
                                    minefield[nr][nc].neighbor = bombCounter(nr, nc);
                                    unsigned char new_tile[1];
                                    new_tile[0] = 3 + minefield[nr][nc].neighbor;
                                    set_bkg_tiles((UINT8)nc + 5, (UINT8)nr + 4, 1, 1, new_tile);
                                    changed = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
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
    set_bkg_data(16, 1, letter_A);
    set_bkg_data(17, 1, letter_C);
    set_bkg_data(18, 1, letter_C_cedilla);
    set_bkg_data(19, 1, letter_D);
    set_bkg_data(20, 1, letter_E);
    set_bkg_data(21, 1, letter_H);
    set_bkg_data(22, 1, letter_I);
    set_bkg_data(23, 1, letter_M);
    set_bkg_data(24, 1, letter_N);
    set_bkg_data(25, 1, letter_O);
    set_bkg_data(26, 1, letter_P);
    set_bkg_data(27, 1, letter_R);
    set_bkg_data(28, 1, letter_B);
    set_bkg_data(29, 1, letter_G);
    set_bkg_data(30, 1, letter_S);
    set_bkg_data(31, 1, letter_T);
    set_bkg_data(32, 1, letter_U);
    set_bkg_data(33, 1, letter_V);
    set_bkg_data(34, 1, symbol_exclamation);
    set_bkg_data(35, 1, symbol_comma);

    set_sprite_data(0, 1, cursor);
    set_sprite_tile(0, 0);

    // Inicia na tela de Start
    drawStartScreen();

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    while (1)
    {
        UINT8 input = joypad();

        // Lógica da tela inicial
        if (current_state == 0)
        {
            if (input & J_UP && menu_option == 1) {
                menu_option = 0;
                cursor_y = 112;
                move_sprite(0, cursor_x, cursor_y);
                delay(150);
            }
            if (input & J_DOWN && menu_option == 0) {
                menu_option = 1;
                cursor_y = 128;
                move_sprite(0, cursor_x, cursor_y);
                delay(150);
            }
            if (input & J_A || input & J_START) {
                if (menu_option == 0) {
                    current_state = 1;
                    resetGame();
                } else {
                    DISPLAY_OFF;
                    while(1) { wait_vbl_done(); } // Trava o jogo simulando que desligou
                }
                delay(200);
            }
            wait_vbl_done();
            continue;
        }

        // Telas de Fim de Jogo (2 = Game Over, 3 = Vitória)
        if (current_state == 2 || current_state == 3)
        {
            if (input & J_UP && menu_option == 1) {
                menu_option = 0;
                cursor_y = 112;
                move_sprite(0, cursor_x, cursor_y);
                delay(150);
            }
            if (input & J_DOWN && menu_option == 0) {
                menu_option = 1;
                cursor_y = 128;
                move_sprite(0, cursor_x, cursor_y);
                delay(150);
            }
            if (input & J_A || input & J_START) {
                if (menu_option == 0) {
                    current_state = 1;
                    resetGame();
                } else {
                    current_state = 0;
                    drawStartScreen();
                }
                delay(200);
            }
            wait_vbl_done();
            continue;
        }

        // Condição de Vitória (90 blocos abertos + 10 bandeiras colocadas)
        if (flagCount == 10 && isShowingCount == 90 && current_state == 1)
        {
            current_state = 3;
            delay(1000); // Pausa pra admirar
            drawEndScreen(1);
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
                    new_tile[0] = 14; // Carimba a bomba explodindo
                    set_bkg_tiles(tile_x, tile_y, 1, 1, new_tile);
                    delay(1000); // Pausa dramática
                    current_state = 2; // Game Over
                    drawEndScreen(0);
                    continue; // Pula o resto
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
                
                // Se foi revelado um zero, abre em cadeia!
                if (minefield[row][col].hasBomb == 0 && minefield[row][col].neighbor == 0) {
                    revealZeros();
                }

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