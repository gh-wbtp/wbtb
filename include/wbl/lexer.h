#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "util.h"

typedef enum
{
    WBLT_EOPEN,
    WBLT_ECLOSE,
    WBLT_TEXT,
} WblTokenType;

typedef struct WblToken WblToken;

struct WblToken
{
    WblTokenType type;
    union
    {
        char *tag;
        char *text;
    };

    void (*free)(WblToken *token);
};

typedef struct WblLexerState WblLexerState;

struct WblLexerState
{
    const char *source;
    size_t source_length;

    WblToken *tokens;
    u32 tokens_length;
    u32 tokens_cap;

    void (*free)(WblLexerState *lexer);
};

bool tokenize(WblLexerState *lexer);
