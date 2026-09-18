#include "wbl/lexer.h"
#include <stdlib.h>
#include <string.h>

void token_free(WblToken *token)
{
    if (token->type == WBLT_EOPEN || token->type == WBLT_ECLOSE)
        free(token->tag);
    else if (token->type == WBLT_TEXT)
        free(token->text);
}

void lexer_free(WblLexerState *lexer)
{
    for (u32 i = 0; i < lexer->tokens_length; i++)
        lexer->tokens[i].free(&lexer->tokens[i]);

    lexer->tokens_length = 0;
    lexer->tokens_cap = 0;
}

WblToken token_new(WblTokenType type, const char *tag, const char *text)
{
    WblToken token = {.type = type, .free = &token_free};
    if (type == WBLT_EOPEN || type == WBLT_ECLOSE)
    {
        token.tag = malloc(strlen(tag) + 1);
        strcpy(token.tag, tag);
    }
    else if (type == WBLT_TEXT)
    {
        token.text = malloc(strlen(text) + 1);
        strcpy(token.text, text);
    }

    return token;
}

WblToken *token_append(WblLexerState *lexer, const WblToken appendee)
{
    WblToken token = {.type = appendee.type, .free = appendee.free};

    if (token.type == WBLT_EOPEN || token.type == WBLT_ECLOSE)
    {
        token.tag = malloc(strlen(appendee.tag) + 1);
        strcpy(token.tag, appendee.tag);
    }
    else if (token.type == WBLT_TEXT)
    {
        token.text = malloc(strlen(appendee.text) + 1);
        strcpy(token.text, appendee.text);
    }

    if (lexer->tokens_length >= lexer->tokens_cap)
    {
        lexer->tokens_cap *= 2;

        WblToken *tokens = realloc(lexer->tokens, sizeof(WblToken) * lexer->tokens_cap);
        if (!tokens)
        {
            lexer->tokens_cap /= 2;
            return NULL;
        }

        lexer->tokens = tokens;
    }

    lexer->tokens[lexer->tokens_length] = token;
    return &lexer->tokens[lexer->tokens_length++];
}

void flush_lit(WblLexerState *lexer, char *temp_lit, u16 *temp_li)
{
    if (*temp_li > 0)
    {
        temp_lit[(*temp_li)++] = '\0';
        token_append(lexer, token_new(WBLT_TEXT, NULL, temp_lit));
        *temp_li = 0;
    }
}

bool tokenize(WblLexerState *lexer)
{
    lexer->tokens_length = 0;
    lexer->tokens_cap = 8;
    lexer->tokens = malloc(sizeof(WblToken) * lexer->tokens_cap);
    lexer->free = &lexer_free;

    char temp_lit[4096];
    u16 temp_li = 0;
    for (size_t i = 0; i < lexer->source_length; i++)
    {
        char c = lexer->source[i];

        if (c == '<')
        {
            flush_lit(lexer, temp_lit, &temp_li);

            if (lexer->source[++i] == '/')
            {
                i++;
                char temp_tag[256];
                u8 temp_ti = 0;
                while (i < lexer->source_length)
                {
                    c = lexer->source[i++];
                    if (c == '>')
                    {
                        i--;
                        break;
                    }

                    temp_tag[temp_ti++] = c;
                }

                temp_tag[temp_ti++] = '\0';
                token_append(lexer, token_new(WBLT_ECLOSE, temp_tag, NULL));
                continue;
            }

            char temp_tag[256];
            u8 temp_ti = 0;
            while (i < lexer->source_length)
            {
                c = lexer->source[i++];
                if (c == '>')
                {
                    i--;
                    break;
                }

                temp_tag[temp_ti++] = c;
            }

            temp_tag[temp_ti++] = '\0';
            token_append(lexer, token_new(WBLT_EOPEN, temp_tag, NULL));
            continue;
        }

        temp_lit[temp_li++] = c;
    }

    flush_lit(lexer, temp_lit, &temp_li);
    return true;
}
