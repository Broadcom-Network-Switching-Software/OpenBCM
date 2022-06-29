
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __MDB_KAPS_TYPES_H__
#define __MDB_KAPS_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <libs/kaps/include/kaps_ad.h>
#include <libs/kaps/include/kaps_db.h>
#include <libs/kaps/include/kaps_hb.h>
#include <libs/kaps/include/kaps_instruction.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_mdb.h>
#include <soc/dnx/mdb_global.h>


typedef struct kaps_db * mdb_kaps_db_t_p;

typedef struct kaps_ad_db * mdb_kaps_ad_db_t_p;

typedef struct kaps_ad * mdb_kaps_ad_entry_t_p;

typedef struct kaps_hb_db * mdb_kaps_hb_db_t_p;

typedef struct kaps_instruction * mdb_kaps_instruction_t_p;



typedef struct {
    mdb_kaps_instruction_t_p inst_p;
} mdb_kaps_instruction_handles_t;

typedef struct {
    mdb_kaps_db_t_p db_p;
    mdb_kaps_hb_db_t_p hb_db_p;
    mdb_kaps_ad_db_t_p ad_db_p_arr[DNX_DATA_MAX_MDB_KAPS_MAX_NOF_RESULT_TYPES];
    mdb_lpm_ad_size_e ad_db_size_arr[DNX_DATA_MAX_MDB_KAPS_MAX_NOF_RESULT_TYPES];
    uint8 is_valid;
} mdb_kaps_db_handles_t;

typedef struct {
    mdb_kaps_db_handles_t* db_info;
    mdb_kaps_instruction_handles_t* search_instruction_info;
    uint32 big_kaps_large_bb_mode[DNX_DATA_MAX_MDB_KAPS_NOF_BIG_BBS_BLK_IDS];
    uint8 (*ad_size_supported)[MDB_NOF_LPM_AD_SIZES];
} mdb_kaps_sw_state_t;


#endif 
