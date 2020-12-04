
/*! \file diag_dnx_oam.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_OAM_H_INCLUDED
#define DIAG_DNX_OAM_H_INCLUDED
/*************
 * INCLUDES  *
 */

/*************
 *  DEFINES  *
 */

/*************
 *  MACROES  *
 */
/* Macro used by diagnostics functions */
#define MAC_PRINT_FMT "%02x:%02x:%02x:%02x:%02x:%02x\n"
#define MAC_PRINT_ARG(mac_addr) mac_addr[0], mac_addr[1], \
        mac_addr[2], mac_addr[3],                         \
        mac_addr[4], mac_addr[5]

#define IPV4_PRINT_FMT "%u.%u.%u.%u"
#define IPV4_PRINT_ARG_LE(addr) ((uint8*)&addr)[3], ((uint8*)&addr)[2],((uint8*)&addr)[1],((uint8*)&addr)[0]
#define IPV4_PRINT_ARG_BE(addr) ((uint8*)&addr)[0], ((uint8*)&addr)[1],((uint8*)&addr)[2],((uint8*)&addr)[3]

#define IPV6_PRINT_FMT "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x"
#define IPV6_PRINT_ARG(addr) ((uint8*)&addr)[0], ((uint8*)&addr)[1], ((uint8*)&addr)[2], ((uint8*)&addr)[3], \
    ((uint8*)&addr)[4], ((uint8*)&addr)[5], ((uint8*)&addr)[6], ((uint8*)&addr)[7], \
    ((uint8*)&addr)[8], ((uint8*)&addr)[9], ((uint8*)&addr)[10], ((uint8*)&addr)[11], \
    ((uint8*)&addr)[12], ((uint8*)&addr)[13], ((uint8*)&addr)[14], ((uint8*)&addr)[15]

#define MAID_PRINT_FMT "%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u.%u"
#define MAID_PRINT_ARG(addr) ((uint8*)&addr)[0], ((uint8*)&addr)[1],((uint8*)&addr)[2],((uint8*)&addr)[3],((uint8*)&addr)[4], ((uint8*)&addr)[5],((uint8*)&addr)[6],((uint8*)&addr)[7],\
   ((uint8*)&addr)[8], ((uint8*)&addr)[9],((uint8*)&addr)[10],((uint8*)&addr)[11],((uint8*)&addr)[12], ((uint8*)&addr)[13],((uint8*)&addr)[14],((uint8*)&addr)[15],\
   ((uint8*)&addr)[16], ((uint8*)&addr)[17],((uint8*)&addr)[18],((uint8*)&addr)[19],((uint8*)&addr)[20], ((uint8*)&addr)[21],((uint8*)&addr)[22],((uint8*)&addr)[23],\
   ((uint8*)&addr)[24], ((uint8*)&addr)[25],((uint8*)&addr)[26],((uint8*)&addr)[27],((uint8*)&addr)[28], ((uint8*)&addr)[29],((uint8*)&addr)[30],((uint8*)&addr)[31],\
   ((uint8*)&addr)[32], ((uint8*)&addr)[33],((uint8*)&addr)[34],((uint8*)&addr)[35],((uint8*)&addr)[36], ((uint8*)&addr)[37],((uint8*)&addr)[38],((uint8*)&addr)[39],\
   ((uint8*)&addr)[40], ((uint8*)&addr)[41],((uint8*)&addr)[42],((uint8*)&addr)[43],((uint8*)&addr)[44], ((uint8*)&addr)[45],((uint8*)&addr)[46],((uint8*)&addr)[47]

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_oam_man;
extern sh_sand_cmd_t sh_dnx_oam_cmds[];

/*************
 * FUNCTIONS *
 */

#endif
