#include <string.h>
#include <stdio.h>
#include "wbl/dom.h"
#include "wbl/lexer.h"

int main(void)
{
    const char *source = "<root>Hey guys! Here's a <link><url>wbtp://wbtp.flappygrant.com/png</url>link</link> to view a PNG.</root>";

    WblLexerState lexer = {.source = source, .source_length = strlen(source)};
    if (!tokenize(&lexer))
    {
        fprintf(stderr, "Failed to tokenize source!\n");
        return 1;
    }

    for (u32 i = 0; i < lexer.tokens_length; i++)
    {
        WblToken *token = &lexer.tokens[i];
        switch (token->type)
        {
        case WBLT_EOPEN:
        {
            printf("EOPEN -> %s\n", token->tag);
            break;
        }

        case WBLT_ECLOSE:
        {
            printf("ECLOSE -> %s\n", token->tag);
            break;
        }

        case WBLT_TEXT:
        {
            printf("TEXT -> \"%s\"\n", token->text);
            break;
        }

        default:
        {
            fprintf(stderr, "Unknown token type! %u\n", token->type);
            return 1;
        }
        }
    }

    WblNode root = wbl_element("root");

    char buf[4096];
    size_t written = wbl_node_stringify(root, buf, 4096);
    printf("%.*s\n", (int)written, buf);

    root.free(&root);
    return 0;
}
