/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        main.c
 * Purpose:     API mode test driver
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "api_mode_yy.h"
#include "api_grammar.tab.h"

#if defined(APIMODE_INCLUDE_MAIN)

static void
usage(void)
{
    fprintf(stderr, "apimode [-h] [-d] [-v]\n");
    fprintf(stderr, "    -h     Print this message\n");
    fprintf(stderr, "    -d     Grammar debug\n");
    fprintf(stderr, "    -v     Verbose messages\n");
}

#define BUF_SIZE 128
static char buf[BUF_SIZE];

static int
parse_callback(api_mode_arg_t *arg, void *user_data)
{
    (void)user_data;
    api_mode_show(0,arg);

    return 0;
}

int
main(int argc, char *argv[])
{
    int c;
    int flags = 0;

    while ((c = getopt(argc, argv, "sdhv")) != EOF) {
        switch (c) {
        case 's':
            flags |= SCAN_DEBUG;
            break;
        case 'd':
            flags |= PARSE_DEBUG;
            break;
        case 'v':
            flags |= PARSE_VERBOSE;
            break;
        case 'h':
        default:
            usage();
            exit(0);
            break;
        }
    }

    for (;;) {
        if (flags & PARSE_VERBOSE) {
            puts("Reading...\n");
        }
        memset(buf, 0, sizeof(buf));
        if ((fgets(buf, sizeof(buf), stdin)) == NULL) {
            break;
        }
        if (flags & PARSE_VERBOSE) {
            puts("Parsing...\n");
        }
        api_mode_parse_string(buf, flags, parse_callback, NULL);
    }
    return 0;
}

#endif /* APIMODE_INCLUDE_MAIN */
