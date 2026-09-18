#define _POSIX_C_SOURCE 199309L
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <signal.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wbl/lexer.h"
#include "wbl/parser.h"
#include "gfx.h"

volatile atomic_bool running = true;

void sigint_callback(int sig)
{
    atomic_store(&running, false);
}

char *read_stdin(size_t *length)
{
    size_t cap = 4096;
    size_t length_i = 0;

    char *buf = malloc(cap);
    if (!buf)
        return NULL;

    while (true)
    {
        size_t available = cap - length_i;
        size_t n = fread(buf + length_i, 1, available, stdin);
        length_i += n;
        if (n < available)
        {
            if (ferror(stdin))
            {
                free(buf);
                return NULL;
            }

            break;
        }

        cap *= 2;
        char *new = realloc(buf, cap);
        if (!new)
        {
            free(buf);
            return NULL;
        }

        buf = new;
    }

    *length = length_i;
    return buf;
}

typedef struct
{
    u8 x, y;
    bool center;
    bool heading;
} RenderState;

void render_node(WblNode *node, RenderState *state)
{
    for (u32 i = 0; i < node->children_length; i++)
    {
        WblNode *child = &node->children[i];
        if (child->type == WBL_TEXT)
        {
            size_t text_length = strlen(child->text);
            u8 text_x = state->center && text_length < WBTBT_WIDTH ? (WBTBT_WIDTH - text_length) / 2 : state->x;
            terminal_text((WbtbtText){.text = child->text, .max_length = WBTBT_NOWRAP, .x = text_x, .y = state->y++});

            if (state->heading)
            {
                char *text = malloc(text_length + 1);
                for (u16 j = 0; j < text_length; j++)
                    text[j] = '-';
                text[text_length] = '\0';
                terminal_text((WbtbtText){.text = text, .max_length = WBTBT_NOWRAP, .x = text_x, .y = state->y++});
                free(text);
            }
        }
        else if (child->type == WBL_ELEMENT)
        {
            bool is_heading = strcmp(child->tag, "h1") == 0 || strcmp(child->tag, "h2") == 0 || strcmp(child->tag, "h3") == 0;
            bool is_center = strcmp(child->tag, "center") == 0;

            if (is_heading)
                state->heading = true;
            else if (is_center)
                state->center = true;

            render_node(child, state);

            if (is_heading)
                state->heading = false;
            else if (is_center)
                state->center = false;
        }
    }
}

int main(void)
{
    if (signal(SIGINT, sigint_callback) == SIG_ERR)
    {
        fprintf(stderr, "Unable to set up SIGINT interceptor! CTRL+C won't properly reset terminal state and free memory.");
        return 1;
    }

    size_t source_length = 0;
    char *source = read_stdin(&source_length);

    WblLexerState lexer = {.source = source, .source_length = source_length};
    if (!tokenize(&lexer))
    {
        fprintf(stderr, "Failed to tokenize source!\n");
        return 1;
    }

    WblParserState parser = {.lexer = &lexer};
    if (!parse(&parser))
    {
        fprintf(stderr, "Failed to parse tokens!\n");
        return 1;
    }

    terminal_init(stdout);

    char buf[4096];
    size_t written = wbl_node_stringify(parser.root, buf, 4095);
    buf[written] = '\0';

    RenderState render_state = {.x = 0, .y = 0};
    render_node(&parser.root, &render_state);

    while (atomic_load(&running))
    {
        terminal_render(stdout);

#ifdef _WIN32
        Sleep(17);
#else
        struct timespec frame_time = {.tv_sec = 0, .tv_nsec = 16666667};
        nanosleep(&frame_time, NULL);
#endif
    }

    terminal_cleanup(stdout);
    parser.free(&parser);
    lexer.free(&lexer);
    return 0;
}
