/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * Linemodule CLI access
 */

#if defined(VXWORKS)
#if defined(BMW) || defined(NSX) || defined(METROCORE)

#include "vxWorks.h"
#include "stdio.h"
#include "sys/socket.h"
#include "net/socketvar.h"
#include "sockLib.h"
#include "ioLib.h"
#include "netinet/in.h"
#include <appl/diag/cmdlist.h>
#include <shared/bsl.h>

#define TELNET_PORT 23

extern int sysSlotIdGet();
extern int atoi(const char *);

/* Debug Stuff */
UINT32 saddr = (192 << 24) | (168 << 16) | (0 << 8) | (1 << 0);
int useslotid = 1;

char lm_console_usage[] =
    "Parameters: <lm slot>\n\tConnect to LM slot.\n";

cmd_result_t
lm_console(int unit, args_t *a)
{
    int retval;
    char   buf[4];
    struct sockaddr_in  sa;
    int lineModSock;
    fd_set rfds;
    char *c;
    UINT32 laddr = (192 << 24) | (168 << 16) | (0 << 8) | (0 << 0);

    if ((c = ARG_GET(a)) == NULL) {
        return(ERROR);
    } else {
        printf("IP addr %d.%d.%d.%d\n",
        (saddr >> 24 ) & 0x000000FF,
        (saddr >> 16 ) & 0x000000FF,
        (saddr >> 8 ) & 0x000000FF,
        (saddr >> 0 ) & 0x000000FF
        );

        laddr |= (sysSlotIdGet() ? (1 << 8) : 0);
        laddr |= ((atoi(c)) & 0x000000FF);
        printf("slot %s IP addr %d.%d.%d.%d\n", c,
        (laddr >> 24 ) & 0x000000FF,
        (laddr >> 16 ) & 0x000000FF,
        (laddr >> 8 ) & 0x000000FF,
        (laddr >> 0 ) & 0x000000FF
        );
        if (useslotid) {
            saddr = laddr;
        }
        if (((laddr & 0x000000FF) < 3 ) || ((laddr & 0x000000FF) > 10 )) {
                cli_out("Slot ID should be 3 .. 10\n");
                return(ERROR);
        }
    }


    if ((lineModSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printf("Socket Create failed\n");
        return (ERROR);
    }

    /* connect to Telnet port */
    bzero ((char *)&sa, sizeof(sa));
    sa.sin_family      = AF_INET;
    sa.sin_port        = htons(TELNET_PORT);
    sa.sin_addr.s_addr = htonl(saddr);
    if (connect(lineModSock, (struct sockaddr *)&sa, sizeof(sa)) < 0 )
    {
        printf("connect failed\n");
        return (ERROR);
    }

    printf("connect success %d\n", lineModSock);


    while(1) {
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        FD_SET(lineModSock, &rfds);
        retval = select(lineModSock + 1, &rfds, NULL, NULL, NULL);

        if (retval < 0) break;

        if (FD_ISSET(0, &rfds)) {
            if (read(0, buf, 1)) {
                write(lineModSock, buf, 1);
            } else {
                break;
            }
        } else if (FD_ISSET(lineModSock, &rfds)) {
            if (read(lineModSock, buf, 1)) {
                write(0, buf, 1);
            } else {
                break;
            }
        }
    }

    close(lineModSock);

    return(OK);
}

#endif /* defined(BMW) || defined(NSX) || defined(METROCORE) */
#endif /* defined(VXWORKS) */

int _bcm_diag_cfmlm_not_empty;
