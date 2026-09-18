#include "gfx.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static char outbuf[WBTBT_HEIGHT][WBTBT_WIDTH];

void terminal_init(FILE *stream)
{
    fprintf(stream, "\033[?1049h\033[2J\033[H\033[?25l");
    fflush(stream);
    terminal_clear();
}

void terminal_cleanup(FILE *stream)
{
    fprintf(stream, "\033[?25h\033[?1049l");
    fflush(stream);
}

void terminal_render(FILE *stream)
{
    for (u8 y = 0; y < WBTBT_HEIGHT; y++)
    {
        fprintf(stream, "\033[%u;1H", y + 1);
        fwrite(outbuf[y], 1, WBTBT_WIDTH, stream);
    }

    fflush(stream);
}

void terminal_getbuf(char buf[WBTBT_HEIGHT][WBTBT_WIDTH])
{
    memcpy(buf, outbuf, WBTBT_CHARS);
}

void terminal_setbuf(const char buf[WBTBT_HEIGHT][WBTBT_WIDTH])
{
    memcpy(outbuf, buf, WBTBT_CHARS);
}

void terminal_clear(void)
{
    for (u8 y = 0; y < WBTBT_HEIGHT; y++)
        for (u8 x = 0; x < WBTBT_WIDTH; x++)
            outbuf[y][x] = ' ';
}

void terminal_text(const WbtbtText text)
{
    size_t text_length = strlen(text.text);
    u8 width = text.max_length > 0 ? text.max_length : text_length;
    u8 height = text.max_length > 0 ? (text_length + width - 1) / width : 1;
    u16 i = 0;
    for (u8 y = text.y; y < text.y + height && i < text_length; y++)
        for (u8 x = text.x; x < text.x + width && i < text_length; x++)
            outbuf[y][x] = text.text[i++];
}
