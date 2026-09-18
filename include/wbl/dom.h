#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "util.h"

typedef enum
{
    WBL_ELEMENT,
    WBL_TEXT,
} WblNodeType;

typedef struct WblNode WblNode;

struct WblNode
{
    WblNodeType type;
    union
    {
        char *tag;
        char *text;
    };

    u32 children_length;
    u32 children_cap;
    WblNode *children;
    WblNode (*copy)(const WblNode node);
    void (*free)(WblNode *node);
};

WblNode wbl_element(const char *tag);

WblNode wbl_text(const char *text);

size_t wbl_node_stringify(const WblNode node, char *buf, size_t buf_length);

WblNode *wbl_node_append(WblNode *node, const WblNode appendee);
