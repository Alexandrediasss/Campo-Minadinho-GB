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

#define ROWS 8
#define COLS 8
#define MINES 10

Block minefield[ROWS][COLS];
UINT8 isShowingCount = 0;
UINT8 isGaming = 0;
UINT8 flagCount = 0;
UINT8 current_state = 0;
UINT8 menu_option = 0;

UINT8 cursor_x = 48;
UINT8 cursor_y = 48;

UINT16 timer_ticks = 0;
UINT16 seconds = 0;
UINT16 random_seed = 0;

void print_text(UINT8 x, UINT8 y, const char* str);
void drawHUD(void);

void draw_block(UINT8 gx, UINT8 gy, UINT8 base_tile) {
    unsigned char tiles[4];
    tiles[0] = base_tile;
    tiles[1] = base_tile + 1;
    tiles[2] = base_tile + 2;
    tiles[3] = base_tile + 3;
    set_bkg_tiles(2 + gx*2, 2 + gy*2, 2, 2, tiles);
}

void drawHUD(void) {
    INT8 f = MINES - flagCount;
    if (f < 0) f = 0;
    char flags_str[4];
    flags_str[0] = '0';
    flags_str[1] = '0' + (f / 10);
    flags_str[2] = '0' + (f % 10);
    flags_str[3] = '\0';
    print_text(2, 0, flags_str);

    char sec_str[4];
    sec_str[0] = '0' + (seconds / 100);
    sec_str[1] = '0' + ((seconds / 10) % 10);
    sec_str[2] = '0' + (seconds % 10);
    sec_str[3] = '\0';
    print_text(15, 0, sec_str);
}


UINT8 bombCounter(UINT8 r, UINT8 c) {
    UINT8 count = 0;
    
    if (r > 0) {
        if (c > 0) {
            if (minefield[r-1][c-1].hasBomb == 1) count++;
        }
        if (minefield[r-1][c].hasBomb == 1) count++;
        if (c+1 < COLS) {
            if (minefield[r-1][c+1].hasBomb == 1) count++;
        }
    }
    
    if (c > 0) {
        if (minefield[r][c-1].hasBomb == 1) count++;
    }
    if (c+1 < COLS) {
        if (minefield[r][c+1].hasBomb == 1) count++;
    }
    
    if (r+1 < ROWS) {
        if (c > 0) {
            if (minefield[r+1][c-1].hasBomb == 1) count++;
        }
        if (minefield[r+1][c].hasBomb == 1) count++;
        if (c+1 < COLS) {
            if (minefield[r+1][c+1].hasBomb == 1) count++;
        }
    }
    
    return count;
}

// Sua função de gerar o mapa adaptada
void startGame(void) {
    isShowingCount = 0;
    isGaming = 0;
    flagCount = 0;

    for(UINT8 i = 0; i < ROWS; i++){
        for(UINT8 j = 0; j < COLS; j++){
            minefield[i][j].hasBomb = 0;
            minefield[i][j].isAppearing = 0;
            minefield[i][j].flag = 0;
            minefield[i][j].neighbor = 0;
        }
    }

    UINT8 bomb_count = 0;
    while(bomb_count < MINES){
        UINT8 num1 = (UINT8)(rand() + sys_time) % ROWS;
        UINT8 num2 = (UINT8)(rand() + sys_time + bomb_count) % COLS;

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

    // Desenha a Happy Face (ID 207 = big_happy_face)
    unsigned char face_map[] = {207, 208, 209, 210};
    set_bkg_tiles(9, 0, 2, 2, face_map);

    // Desenha o Tabuleiro (ID 151 = big_closed)
    for(UINT8 gy = 0; gy < ROWS; gy++) {
        for(UINT8 gx = 0; gx < COLS; gx++) {
            draw_block(gx, gy, 151);
        }
    }

    timer_ticks = 0;
    seconds = 0;
    drawHUD();

    cursor_x = 24;
    cursor_y = 32;
    set_sprite_tile(0, 0);
    set_sprite_tile(1, 1);
    set_sprite_tile(2, 2);
    set_sprite_tile(3, 3);
    move_sprite(0, cursor_x, cursor_y);
    move_sprite(1, cursor_x+8, cursor_y);
    move_sprite(2, cursor_x, cursor_y+8);
    move_sprite(3, cursor_x+8, cursor_y+8);
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
    set_sprite_tile(0, 4);
    move_sprite(0, cursor_x, cursor_y);
    move_sprite(1, 0, 0);
    move_sprite(2, 0, 0);
    move_sprite(3, 0, 0);
}


void print_text(UINT8 x, UINT8 y, const char* str) {
    unsigned char tiles[20];
    UINT8 i = 0;
    while(str[i] != '\0' && i < 20) {
        char c = str[i];
        if (c >= 'A' && c <= 'Z') tiles[i] = 100 + (c - 'A');
        else if (c >= 'a' && c <= 'z') tiles[i] = 100 + (c - 'a');
        else if (c >= '0' && c <= '9') tiles[i] = 126 + (c - '0');
        else if (c == '.') tiles[i] = 136;
        else if (c == '/') tiles[i] = 137;
        else if (c == '-') tiles[i] = 138;
        else if (c == ':') tiles[i] = 139;
        else tiles[i] = 140; // space
        i++;
    }
    set_bkg_tiles(x, y, i, 1, tiles);
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

    // TÍTULO (Movido para cima para dar espaço)
    unsigned char campo_text[] = {17, 16, 23, 26, 25};
    set_bkg_tiles(7, 2, 5, 1, campo_text);
    unsigned char minadinho_text[] = {23, 22, 24, 16, 19, 22, 24, 21, 25};
    set_bkg_tiles(5, 4, 9, 1, minadinho_text);

    // CRÉDITOS
    print_text(2, 7, "GITHUB.COM/");
    print_text(2, 8, "ALEXANDREDIASSS");
    print_text(2, 10, "LINKEDIN.COM/IN/");
    print_text(2, 11, "ALEXANDRE-DIASS");

    // COMEÇAR
    unsigned char comecar_text[] = {17, 25, 23, 20, 18, 16, 27};
    set_bkg_tiles(6, 15, 7, 1, comecar_text);

    // Mostra o cursor na tela inicial
    menu_option = 0;
    cursor_x = 40;
    cursor_y = 136;
    set_sprite_tile(0, 4);
    move_sprite(0, cursor_x, cursor_y);
    move_sprite(1, 0, 0);
    move_sprite(2, 0, 0);
    move_sprite(3, 0, 0);
}

void revealZeros(void) {
    UINT8 changed = 1;
    INT8 r, c, dr, dc, nr, nc;
    while (changed) {
        changed = 0;
        for (r = 0; r < ROWS; r++) {
            for (c = 0; c < COLS; c++) {
                if (minefield[r][c].isAppearing == 1 && minefield[r][c].hasBomb == 0 && minefield[r][c].neighbor == 0) {
                    for (dr = -1; dr <= 1; dr++) {
                        for (dc = -1; dc <= 1; dc++) {
                            if (dr == 0 && dc == 0) continue;
                            nr = r + dr;
                            nc = c + dc;
                            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {
                                if (minefield[nr][nc].isAppearing == 0 && minefield[nr][nc].flag == 0) {
                                    minefield[nr][nc].isAppearing = 1;
                                    isShowingCount++;
                                    minefield[nr][nc].neighbor = bombCounter(nr, nc);
                                    UINT8 base_id = (minefield[nr][nc].neighbor == 0) ? 155 : (155 + minefield[nr][nc].neighbor * 4);
                                    draw_block(nc, nr, base_id);
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

    set_bkg_data(100, 41, system_font); // Carrega a fonte
    set_bkg_data(151, 4, big_closed_block);   
    set_bkg_data(155, 4, big_empty_block);    
    set_bkg_data(159, 4, big_block_1);    
    set_bkg_data(163, 4, big_block_2);   
    set_bkg_data(167, 4, big_block_3);   
    set_bkg_data(171, 4, big_block_4);   
    set_bkg_data(175, 4, big_block_5);   
    set_bkg_data(179, 4, big_block_6);   
    set_bkg_data(183, 4, big_block_7);   
    set_bkg_data(187, 4, big_block_8);   
    set_bkg_data(191, 4, big_flag);    
    set_bkg_data(195, 4, big_bomb);    
    set_bkg_data(199, 4, big_bomb_explosion); 
    set_bkg_data(203, 4, big_wrong_flag);  
    set_bkg_data(207, 4, big_happy_face);  


    set_sprite_data(0, 1, cursor_tl);
    set_sprite_data(1, 1, cursor_tr);
    set_sprite_data(2, 1, cursor_bl);
    set_sprite_data(3, 1, cursor_br);
    set_sprite_data(4, 1, menu_arrow);
    set_sprite_tile(0, 0);
    set_sprite_tile(1, 1);
    set_sprite_tile(2, 2);
    set_sprite_tile(3, 3);

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
            random_seed++;
            if (input & J_A || input & J_START) {
                initrand(random_seed);
                current_state = 1;
                resetGame();
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
        if (flagCount == MINES && isShowingCount == (ROWS * COLS - MINES) && current_state == 1)
        {
            current_state = 3;
            delay(1000); // Pausa pra admirar
            drawEndScreen(1);
            continue;
        }

        // Timer is handled in delay()

        // Movimento
        if (input & J_UP) {
            if (cursor_y > 32) { cursor_y -= 16; delay(150); }
            else if (cursor_y == 32 && cursor_x >= 72 && cursor_x <= 104) {
                cursor_y = 16; cursor_x = 80; delay(150);
            }
        }
        if (input & J_DOWN) {
            if (cursor_y == 16) { cursor_y = 32; cursor_x = 80; delay(150); }
            else if (cursor_y < (32 + (ROWS-1)*16)) { cursor_y += 16; delay(150); }
        }
        if (input & J_LEFT && cursor_x > 24 && cursor_y >= 32) { cursor_x -= 16; delay(150); }
        if (input & J_RIGHT && cursor_x < (24 + (COLS-1)*16) && cursor_y >= 32) { cursor_x += 16; delay(150); }

        move_sprite(0, cursor_x, cursor_y);
        move_sprite(1, cursor_x+8, cursor_y);
        move_sprite(2, cursor_x, cursor_y+8);
        move_sprite(3, cursor_x+8, cursor_y+8);

        // PASSO 3: Lógica de Revelação Completa (Botão A)
        if (input & J_A)
        {
            if (cursor_y == 16) {
                current_state = 1;
                resetGame();
                delay(200);
                continue;
            }

            UINT8 col = (cursor_x - 24) / 16;
            UINT8 row = (cursor_y - 32) / 16;

            if (minefield[row][col].isAppearing == 0 && minefield[row][col].flag == 0)
            {
                minefield[row][col].isAppearing = 1;
                isShowingCount++;
                
                if (minefield[row][col].hasBomb == 1)
                {
                    // Explodiu! 
                    draw_block(col, row, 199); // 199 = big_bomb_explosion
                    delay(1000); // Pausa dramática
                    current_state = 2; // Game Over
                    drawEndScreen(0);
                    continue; // Pula o resto
                }
                else
                {
                    minefield[row][col].neighbor = bombCounter(row, col);
                    UINT8 base_id = (minefield[row][col].neighbor == 0) ? 155 : (155 + minefield[row][col].neighbor * 4);
                    draw_block(col, row, base_id);
                }
                
                // Se foi revelado um zero, abre em cadeia!
                if (minefield[row][col].hasBomb == 0 && minefield[row][col].neighbor == 0) {
                    revealZeros();
                }

                delay(200); 
            }
        }

        // AÇÃO 2: COLOCAR/TIRAR BANDEIRA (Botão B)
        if (input & J_B)
        {
            UINT8 col = (cursor_x - 24) / 16;
            UINT8 row = (cursor_y - 32) / 16;

            if (minefield[row][col].isAppearing == 0)
            {
                if (minefield[row][col].flag == 1)
                {
                    minefield[row][col].flag = 0;
                    flagCount--;
                    draw_block(col, row, 151); // 151 = big_closed
                }
                else
                {
                    minefield[row][col].flag = 1;
                    flagCount++;
                    draw_block(col, row, 191); // 191 = big_flag
                }
                drawHUD();
                delay(200); 
            }
        }
        wait_vbl_done();
    }
}