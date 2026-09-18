#include <stdio.h>
#include "wbl.h"

int main(void)
{
    WblNode root = wbl_element("root");
    wbl_node_append(&root, wbl_text("Hey guys! Here's a "));

    WblNode *link = wbl_node_append(&root, wbl_element("link"));
    WblNode *url = wbl_node_append(link, wbl_element("url"));
    wbl_node_append(url, wbl_text("wbtp://wbtp.flappygrant.com/png"));
    wbl_node_append(link, wbl_text("link"));

    wbl_node_append(&root, wbl_text(" to view a PNG."));

    char buf[4096];
    size_t written = wbl_node_stringify(root, buf, 4096);
    printf("%.*s\n", (int)written, buf);

    root.free(&root);
    return 0;
}
