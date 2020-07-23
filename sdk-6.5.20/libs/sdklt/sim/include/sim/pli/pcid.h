/*! \file pcid.h
 *
 * PCID API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef PCID_H
#define PCID_H

#include <time.h>

/* Return values for process request */
#define PR_REQUEST_HANDLED      1
#define PR_NO_REQUEST           2
#define PR_ALL_DONE             0
#define PR_ERROR                -1

extern int
pcid_setup_socket(int port);

extern int
pcid_wait_for_cnxn(int sockfd);

extern void
pcid_close_cnxn(int sockfd);

extern int
pcid_process_request(int sockfd, struct timeval *tmout);

#endif /* PCID_H */
