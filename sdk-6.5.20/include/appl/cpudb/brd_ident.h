/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        brd_ident.h
 * Purpose:     CPUDB base board identifiers
 */

#ifndef   _BRD_IDENT_H_
#define   _BRD_IDENT_H_

typedef enum {
    cpudb_board_id_unknown,
    cpudb_board_id_cfm_xgs1,
    cpudb_board_id_cfm_xgs2,
    cpudb_board_id_draco_b2b,
    cpudb_board_id_galahad,
    cpudb_board_id_guenevere,
    cpudb_board_id_lancelot,
    cpudb_board_id_lm24g,
    cpudb_board_id_lm2x,
    cpudb_board_id_lm6x,
    cpudb_board_id_lm_xgs1_6x,
    cpudb_board_id_lm_xgs2_48g,
    cpudb_board_id_lm_xgs2_6x,
    cpudb_board_id_lm_xgs3_12x,
    cpudb_board_id_lm_xgs3_48g,
    cpudb_board_id_lynxalot,
    cpudb_board_id_magnum,
    cpudb_board_id_merlin,
    cpudb_board_id_sdk_xgs2_12g,
    cpudb_board_id_sdk_xgs3_12g,
    cpudb_board_id_sdk_xgs3_24g,
    cpudb_board_id_sl24f2g,
    cpudb_board_id_tucana,
    cpudb_board_id_white_knight,
    cpudb_board_id_cfm_xgs3,
    cpudb_board_id_lm_56800_12x,
    cpudb_board_id_sdk_xgs3_48f,
    cpudb_board_id_sdk_xgs3_48g,
    cpudb_board_id_sdk_xgs3_20x,
    cpudb_board_id_sdk_xgs3_16h,
    cpudb_board_id_sdk_xgs3_48g5g,
    cpudb_board_id_sdk_xgs3_48g2,
    cpudb_board_id_count /* last */
} CPUDB_BOARD_ID;

#endif /* _BRD_IDENT_H_ */
