#include <gb/gb.h>
#include "sprites.h"

// Initial cursor positions in pixels
// The 10x10 grid is centered, starting at tile column 5 and row 4.
// X Pixel = (5 tiles * 8 pixels) + 8 (hardware offset) = 48
// Y Pixel = (4 tiles * 8 pixels) + 16 (hardware offset) = 48
UINT8 cursor_x = 48; 
UINT8 cursor_y = 48;

void main() {
    // 1. Loads tile data into VRAM
    set_bkg_data(0, 1, closed_block);

    // 2. Creates a 10x10 map (100 tiles total)
    unsigned char map[100];
    UINT16 i;
    for (i = 0; i < 100; i++) {
        map[i] = 0; // Fills everything with ID 0 (closed_block)
    }

    // 3. Draws the 10x10 map centered at column 5, row 4
    set_bkg_tiles(5, 4, 10, 10, map); 

    // 4. Loads sprite graphics and configures Sprite 0
    set_sprite_data(0, 1, cursor);
    set_sprite_tile(0, 0);
    move_sprite(0, cursor_x, cursor_y); 

    // 5. Turns on the display layers
    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;

    // 6. Main game loop
    while(1) {
        // Reads joypad input
        UINT8 input = joypad();

        // Movement logic (8 pixels per step)
        if (input & J_UP) {
            cursor_y -= 8;
            delay(150); // Delay prevents sliding
        }
        if (input & J_DOWN) {
            cursor_y += 8;
            delay(150);
        }
        if (input & J_LEFT) {
            cursor_x -= 8;
            delay(150);
        }
        if (input & J_RIGHT) {
            cursor_x += 8;
            delay(150);
        }

        // Updates sprite position
        move_sprite(0, cursor_x, cursor_y);

        // Waits for Vertical Blank (syncs to ~60 FPS)
        wait_vbl_done(); 
    }
}