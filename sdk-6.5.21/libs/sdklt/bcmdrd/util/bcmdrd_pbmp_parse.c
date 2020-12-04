/*! \file bcmdrd_pbmp_parse.c
 *
 * Utility functions used for parsing a port list string into BCMDRD
 * port bitmpa struct bcmdrd_pbmp_t.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_types.h>

int
bcmdrd_pbmp_parse(const char *str, bcmdrd_pbmp_t *pbmp)
{
    int port = -1;
    int pstart = -1;
    char ch;
    const char *cptr = str;

    if (str == NULL || pbmp == NULL) {
        return -1;
    }

    BCMDRD_PBMP_CLEAR(*pbmp);

    do {
        ch = *cptr++;
        if (ch >= '0' && ch <= '9') {
            if (port < 0) {
                port = 0;
            }
            port = (port * 10) + (ch - '0');
            if (port >= BCMDRD_CONFIG_MAX_PORTS) {
                return -1;
            }
        } else {
            if (pstart >= 0) {
                if (pstart > port) {
                    return -1;
                }
                while (pstart < port) {
                    BCMDRD_PBMP_PORT_ADD(*pbmp, pstart);
                    pstart++;
                }
                pstart = -1;
            }
            if (ch == ',' || ch == 0) {
                if (port < 0) {
                    return -1;
                }
                BCMDRD_PBMP_PORT_ADD(*pbmp, port);
                port = -1;
            } else if (ch == '-') {
                if (port < 0) {
                    return -1;
                }
                pstart = port;
                port = -1;
            } else {
                return -1;
            }
        }
    } while (ch != 0);

    return 0;
}
