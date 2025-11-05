#include <stdio.h>
#include <terminalgfx.h>
#include <stdint.h>

int main() {
    tgfx_init();
    tgfx_startInput();

    tgfx_fb_init(80, 20);
    tgfx_fb_print(3, 4, (char*)utf8string(0x1f30d));
    tgfx_fb_box();
    tgfx_fb_print(4, 4, "Hello, World");
    tgfx_fb_render();

    tgfx_terminate();
    return 0;
}
