
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __MDB_KAPS_ACCESS_H__
#define __MDB_KAPS_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_kaps_types.h>
#include <libs/kaps/include/kaps_ad.h>
#include <libs/kaps/include/kaps_db.h>
#include <libs/kaps/include/kaps_hb.h>
#include <libs/kaps/include/kaps_instruction.h>



typedef int (*mdb_kaps_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*mdb_kaps_db_init_cb)(
    int unit);


typedef int (*mdb_kaps_db_db_info_set_cb)(
    int unit, uint32 db_info_idx_0, CONST mdb_kaps_db_handles_t *db_info);


typedef int (*mdb_kaps_db_db_info_get_cb)(
    int unit, uint32 db_info_idx_0, mdb_kaps_db_handles_t *db_info);


typedef int (*mdb_kaps_db_search_instruction_info_set_cb)(
    int unit, uint32 search_instruction_info_idx_0, CONST mdb_kaps_instruction_handles_t *search_instruction_info);


typedef int (*mdb_kaps_db_search_instruction_info_get_cb)(
    int unit, uint32 search_instruction_info_idx_0, mdb_kaps_instruction_handles_t *search_instruction_info);


typedef int (*mdb_kaps_db_big_kaps_large_bb_mode_set_cb)(
    int unit, uint32 big_kaps_large_bb_mode_idx_0, uint32 big_kaps_large_bb_mode);


typedef int (*mdb_kaps_db_big_kaps_large_bb_mode_get_cb)(
    int unit, uint32 big_kaps_large_bb_mode_idx_0, uint32 *big_kaps_large_bb_mode);




typedef struct {
    mdb_kaps_db_db_info_set_cb set;
    mdb_kaps_db_db_info_get_cb get;
} mdb_kaps_db_db_info_cbs;


typedef struct {
    mdb_kaps_db_search_instruction_info_set_cb set;
    mdb_kaps_db_search_instruction_info_get_cb get;
} mdb_kaps_db_search_instruction_info_cbs;


typedef struct {
    mdb_kaps_db_big_kaps_large_bb_mode_set_cb set;
    mdb_kaps_db_big_kaps_large_bb_mode_get_cb get;
} mdb_kaps_db_big_kaps_large_bb_mode_cbs;


typedef struct {
    mdb_kaps_db_is_init_cb is_init;
    mdb_kaps_db_init_cb init;
    
    mdb_kaps_db_db_info_cbs db_info;
    
    mdb_kaps_db_search_instruction_info_cbs search_instruction_info;
    
    mdb_kaps_db_big_kaps_large_bb_mode_cbs big_kaps_large_bb_mode;
} mdb_kaps_db_cbs;






const char *
mdb_kaps_ip_db_id_e_get_name(mdb_kaps_ip_db_id_e value);




extern mdb_kaps_db_cbs mdb_kaps_db;

#endif 
