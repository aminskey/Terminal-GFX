#include <stdio.h>
#include <terminalgfx.h>

int main() {
    tgfx_init();
    tgfx_startInput();

    // Create a 40x10 framebuffer
    tgfx_fb_init(80, 40);

    tgfx_cls();
    tgfx_toggleCursor(false);

    // Clear framebuffer with spaces
    tgfx_fb_clear(' ');

    // Draw text using the framebuffer string printer
    tgfx_fb_print(2, 2, "Hello, TerminalGFX!");

    // Draw a border
    for (int i = 0; i < 80; i++) {
        tgfx_fb_put(i, 0, '-');
        tgfx_fb_put(i, 9, '-');
    }
    for (int i = 0; i < 40; i++) {
        tgfx_fb_put(0, i, '|');
        tgfx_fb_put(39, i, '|');
    }

    // Render framebuffer to screen
    tgfx_fb_render();
    printf("\nPress 'q' to quit.\n");

    // Main loop
    while (1) {
        int key = tgfx_readInput(1);
        if (key == 'q') break;
        tgfx_tick(60);
    }

    tgfx_fb_quit();
    tgfx_terminate();
    return 0;
}
