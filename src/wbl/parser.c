#include "wbl/parser.h"
#include <stdlib.h>
#include <string.h>

void parser_free(WblParserState *parser)
{
    parser->root.free(&parser->root);
}

bool push_parent(WblNode ***parents, u32 *length, u32 *capacity, WblNode *parent)
{
    if (*length >= *capacity)
    {
        *capacity * 2;
        WblNode **new_parents = realloc(*parents, sizeof(WblNode *) * *capacity);
        if (!new_parents)
        {
            *capacity /= 2;
            return false;
        }

        *parents = new_parents;
    }

    (*parents)[(*length)++] = parent;
    return true;
}

WblNode *current_parent(WblNode **parents, u32 length)
{
    return length > 0 ? parents[length - 1] : NULL;
}

void pop_parent(u32 *length)
{
    if (*length > 0)
        (*length)--;
}

bool parse(WblParserState *parser)
{
    parser->free = &parser_free;
    parser->root = wbl_element("root");

    u32 parents_length = 0;
    u32 parents_capacity = 8;
    WblNode **parents = malloc(sizeof(WblNode *) * parents_capacity);
    if (!parents)
        return false;

    if (!push_parent(&parents, &parents_length, &parents_capacity, &parser->root))
    {
        free(parents);
        return false;
    }

    for (u32 i = 0; i < parser->lexer->tokens_length; i++)
    {
        WblToken *token = &parser->lexer->tokens[i];
        WblNode *parent = current_parent(parents, parents_length);
        if (!parent)
        {
            free(parents);
            return false;
        }

        switch (token->type)
        {
        case WBLT_EOPEN:
        {
            WblNode *node = wbl_node_append(parent, wbl_element(token->tag));
            if (!node)
            {
                free(parents);
                return false;
            }

            if (!push_parent(&parents, &parents_length, &parents_capacity, node))
            {
                free(parents);
                return false;
            }

            break;
        }

        case WBLT_ECLOSE:
        {
            if (parents_length <= 1)
            {
                free(parents);
                return false;
            }

            WblNode *node = current_parent(parents, parents_length);
            if (strcmp(node->tag, token->tag) != 0)
            {
                free(parents);
                return false;
            }

            pop_parent(&parents_length);
            break;
        }

        case WBLT_TEXT:
        {
            if (!wbl_node_append(parent, wbl_text(token->text)))
            {
                free(parents);
                return false;
            }

            break;
        }

        default:
        {
            free(parents);
            return false;
        }
        }
    }

    free(parents);
    return parents_length == 1;
}
