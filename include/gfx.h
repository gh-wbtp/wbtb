#pragma once

#include <stdbool.h>
#include <stdio.h>
#include "util.h"

#define WBTBT_WIDTH 80
#define WBTBT_HEIGHT 25
#define WBTBT_CHARS (WBTBT_WIDTH * WBTBT_HEIGHT)
#define WBTBT_NOWRAP 0

typedef struct
{
    const char *text;
    u16 max_length;
    u8 x, y;
} WbtbtText;

void terminal_init(FILE *stream);

void terminal_cleanup(FILE *stream);

void terminal_render(FILE *stream);

void terminal_getbuf(char buf[WBTBT_HEIGHT][WBTBT_WIDTH]);

void terminal_setbuf(const char buf[WBTBT_HEIGHT][WBTBT_WIDTH]);

void terminal_clear(void);

void terminal_text(const WbtbtText text);
