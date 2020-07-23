/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        brd_ident.c
 * Purpose:     Stack application board identifier functions
 */

#include <sal/core/libc.h>
#include <bcm/error.h>
#include <appl/cpudb/brd_ident.h>

typedef struct {
    char *name;
    CPUDB_BOARD_ID id;
} CPUDB_BOARD_ID_MAP;

static CPUDB_BOARD_ID_MAP board_id_map[] = {
    { "cfm_xgs1", cpudb_board_id_cfm_xgs1 },
    { "cfm_xgs2", cpudb_board_id_cfm_xgs2 },
    { "draco_b2b", cpudb_board_id_draco_b2b },
    { "galahad", cpudb_board_id_galahad },
    { "guenevere", cpudb_board_id_guenevere },
    { "lancelot", cpudb_board_id_lancelot },
    { "lm24g", cpudb_board_id_lm24g },
    { "lm2x", cpudb_board_id_lm2x },
    { "lm6x", cpudb_board_id_lm6x },
    { "lm_xgs1_6x", cpudb_board_id_lm_xgs1_6x },
    { "lm_xgs2_48g", cpudb_board_id_lm_xgs2_48g },
    { "lm_xgs2_6x", cpudb_board_id_lm_xgs2_6x },
    { "lm_xgs3_12x", cpudb_board_id_lm_xgs3_12x },
    { "lm_xgs3_48g", cpudb_board_id_lm_xgs3_48g },
    { "lynxalot", cpudb_board_id_lynxalot },
    { "magnum", cpudb_board_id_magnum },
    { "merlin", cpudb_board_id_merlin },
    { "sdk_xgs2_12g", cpudb_board_id_sdk_xgs2_12g },
    { "sdk_xgs3_12g", cpudb_board_id_sdk_xgs3_12g },
    { "sdk_xgs3_24g", cpudb_board_id_sdk_xgs3_24g },
    { "sl24f2g", cpudb_board_id_sl24f2g },
    { "tucana", cpudb_board_id_tucana },
    { "white_knight", cpudb_board_id_white_knight },
    { "cfm_xgs3", cpudb_board_id_cfm_xgs3 },
    { "lm_56800_12x", cpudb_board_id_lm_56800_12x },
    { "sdk_xgs3_48f", cpudb_board_id_sdk_xgs3_48f},
    { "sdk_xgs3_48g", cpudb_board_id_sdk_xgs3_48g},
    { "sdk_xgs3_20x", cpudb_board_id_sdk_xgs3_20x},
    { "sdk_xgs3_16h", cpudb_board_id_sdk_xgs3_16h},
    { "sdk_xgs3_48g5g", cpudb_board_id_sdk_xgs3_48g5g},
    { "sdk_xgs3_48g2", cpudb_board_id_sdk_xgs3_48g2}
};

int cpudb_board_id_get(const char *idstring, CPUDB_BOARD_ID *board_id)
{
    int i;

    for ( i=0; i<COUNTOF(board_id_map); i++ ) {
        if (!sal_strcmp(idstring, board_id_map[i].name)) {
            *board_id = board_id_map[i].id;
            return BCM_E_NONE;
        }
    }
    return BCM_E_NOT_FOUND;
}

int cpudb_board_idstr_get(CPUDB_BOARD_ID board_id, char **board_idstr)
{
    int i;

    for ( i=0; i<COUNTOF(board_id_map); i++ ) {
        if (board_id == board_id_map[i].id) {
            *board_idstr = board_id_map[i].name;
            return BCM_E_NONE;
        }
    }
    return BCM_E_NOT_FOUND;
}

