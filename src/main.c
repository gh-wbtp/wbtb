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
    terminal_text((WbtbtText){.text = buf, .max_length = WBTBT_WIDTH - 10, .x = 10, .y = 10});

    while (atomic_load(&running))
        terminal_render(stdout);

    terminal_cleanup(stdout);
    parser.free(&parser);
    lexer.free(&lexer);
    return 0;
}
