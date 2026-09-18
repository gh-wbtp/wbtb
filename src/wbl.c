#include "wbl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void node_free(WblNode *node)
{
    if (node->type == WBL_ELEMENT)
    {
        free(node->tag);

        for (u32 i = 0; i < node->children_length; i++)
            node->children[i].free(&node->children[i]);

        free(node->children);
        node->children_length = 0;
        node->children_cap = 0;
    }
    else if (node->type == WBL_TEXT)
        free(node->text);
}

WblNode node_copy(const WblNode node)
{
    if (node.type == WBL_TEXT)
        return wbl_text(node.text);

    WblNode copy = wbl_element(node.tag);
    for (u32 i = 0; i < node.children_length; i++)
        wbl_node_append(&copy, node.children[i]);

    return copy;
}

WblNode wbl_element(const char *tag)
{
    WblNode node = {.type = WBL_ELEMENT, .tag = malloc(strlen(tag) + 1)};
    strcpy(node.tag, tag);
    node.children_length = 0;
    node.children_cap = 4;
    node.children = malloc(sizeof(WblNode) * node.children_cap);
    node.copy = &node_copy;
    node.free = &node_free;
    return node;
}

WblNode wbl_text(const char *text)
{
    WblNode node = {.type = WBL_TEXT, .text = malloc(strlen(text) + 1)};
    strcpy(node.text, text);
    node.copy = &node_copy;
    node.free = &node_free;
    return node;
}

size_t wbl_node_stringify(const WblNode node, char *buf, size_t buf_length)
{
    if (node.type == WBL_ELEMENT)
    {
        size_t buf_i = 0;

        size_t length_l = 2 + strlen(node.tag);
        if (buf_i + length_l >= buf_length)
            return buf_i;

        char *temp_l = malloc(length_l + 1);
        snprintf(temp_l, length_l + 1, "<%s>", node.tag);
        memcpy(buf + buf_i, temp_l, length_l);
        free(temp_l);
        buf_i += length_l;

        for (u32 i = 0; i < node.children_length; i++)
            buf_i += wbl_node_stringify(node.children[i], buf + buf_i, buf_length - buf_i);

        size_t length_r = 3 + strlen(node.tag);
        if (buf_i + length_r >= buf_length)
            return buf_i;

        char *temp_r = malloc(length_r + 1);
        snprintf(temp_r, length_r + 1, "</%s>", node.tag);
        memcpy(buf + buf_i, temp_r, length_r);
        free(temp_r);
        buf_i += length_r;
        return buf_i;
    }
    else if (node.type == WBL_TEXT)
    {
        size_t text_length = strlen(node.text);
        memcpy(buf, node.text, text_length);
        return text_length;
    }

    return 0;
}

WblNode *wbl_node_append(WblNode *node, const WblNode appendee)
{
    if (node->children_length >= node->children_cap)
    {
        node->children_cap *= 2;

        WblNode *children = realloc(node->children, sizeof(WblNode) * node->children_cap);
        if (!children)
        {
            node->children_cap /= 2;
            return NULL;
        }

        node->children = children;
    }

    node->children[node->children_length] = appendee.copy(appendee);
    return &node->children[node->children_length++];
}
