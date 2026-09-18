#include <string.h>
#include <stdio.h>
#include "wbl/lexer.h"
#include "wbl/parser.h"

int main(void)
{
    const char *source = "Hey guys! Here's a <link><url>wbtp://wbtp.flappygrant.com/png</url>link</link> to view a PNG.";

    WblLexerState lexer = {.source = source, .source_length = strlen(source)};
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

    char buf[4096];
    size_t written = wbl_node_stringify(parser.root, buf, 4096);
    printf("%.*s\n", (int)written, buf);

    parser.free(&parser);
    lexer.free(&lexer);
    return 0;
}
