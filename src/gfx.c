#include "gfx.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static char outbuf[WBTBT_HEIGHT][WBTBT_WIDTH];
static WbtbtStyling stylingbuf[WBTBT_HEIGHT][WBTBT_WIDTH];

WbtbtStyling styling_empty(void)
{
    return (WbtbtStyling){
        .bold = false,
        .italic = false,
        .underline = false,
        .has_color = false,
        .color = {.r = 0, .g = 0, .b = 0},
    };
}

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
    WbtbtStyling previous = styling_empty();
    for (u8 y = 0; y < WBTBT_HEIGHT; y++)
    {
        fprintf(stream, "\033[%u;1H", y + 1);
        for (u8 x = 0; x < WBTBT_WIDTH; x++)
        {
            WbtbtStyling current = stylingbuf[y][x];
            if (memcmp(&current, &previous, sizeof(WbtbtStyling)) != 0)
            {
                fprintf(stream, "\033[0m");

                if (current.bold)
                    fprintf(stream, "\033[1m");

                if (current.italic)
                    fprintf(stream, "\033[3m");

                if (current.underline)
                    fprintf(stream, "\033[4m");

                fprintf(stream, "\033[38;2;%u;%u;%um", current.color.r, current.color.g, current.color.b);
                previous = current;
            }

            fputc(outbuf[y][x], stream);
        }
    }

    fprintf(stream, "\033[0m");
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
    for (u8 y = 0; y < WBTBT_HEIGHT; y++)
        for (u8 x = 0; x < WBTBT_WIDTH; x++)
            stylingbuf[y][x] = styling_empty();
}

void terminal_text(const WbtbtText text, const WbtbtStyling styling)
{
    size_t text_length = strlen(text.text);
    u8 width = text.max_length > 0 ? text.max_length : text_length;
    u8 height = text.max_length > 0 ? (text_length + width - 1) / width : 1;
    u16 i = 0;
    for (u8 y = text.y; y < text.y + height && i < text_length; y++)
        for (u8 x = text.x; x < text.x + width && i < text_length; x++)
        {
            outbuf[y][x] = text.text[i++];
            stylingbuf[y][x] = styling;
        }
}
