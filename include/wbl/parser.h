#pragma once

#include "wbl/ast.h"
#include "wbl/lexer.h"

typedef struct WblParserState WblParserState;

struct WblParserState
{
    WblLexerState *lexer;
    WblNode root;

    void (*free)(WblParserState *parser);
};

bool parse(WblParserState *parser);
