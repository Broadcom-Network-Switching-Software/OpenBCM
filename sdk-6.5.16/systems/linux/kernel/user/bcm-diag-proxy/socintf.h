/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        socintf.h
 * Purpose:     
 */

#ifndef   _SOCINTF_H_
#define   _SOCINTF_H_

extern int setup_socket(int port);
extern int wait_for_cnxn(int sockfd);
extern int conn(char* servier, int port); 

#endif /* _SOCINTF_H_ */
