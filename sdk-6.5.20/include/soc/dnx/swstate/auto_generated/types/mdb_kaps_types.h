
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __MDB_KAPS_TYPES_H__
#define __MDB_KAPS_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <libs/kaps/include/kaps_ad.h>
#include <libs/kaps/include/kaps_db.h>
#include <libs/kaps/include/kaps_hb.h>
#include <libs/kaps/include/kaps_instruction.h>



#define MDB_LPM_NOF_INSTRUCTIONS 1


#define MDB_LPM_MAX_NOF_BIG_KAPS_BB_BLK_IDS 14



typedef struct kaps_db * mdb_kaps_db_t_p;

typedef struct kaps_ad_db * mdb_kaps_ad_db_t_p;

typedef struct kaps_hb_db * mdb_kaps_hb_db_t_p;

typedef struct kaps_instruction * mdb_kaps_instruction_t_p;




typedef enum {
    
    MDB_KAPS_IP_PRIVATE_DB_ID = 0,
    
    MDB_KAPS_IP_PUBLIC_DB_ID = 1,
    MDB_KAPS_IP_NOF_DB
} mdb_kaps_ip_db_id_e;




typedef struct {
    mdb_kaps_instruction_t_p inst_p;
} mdb_kaps_instruction_handles_t;


typedef struct {
    mdb_kaps_db_t_p db_p;
    mdb_kaps_ad_db_t_p ad_db_p;
    mdb_kaps_hb_db_t_p hb_db_p;
    uint8 is_valid;
} mdb_kaps_db_handles_t;


typedef struct {
    mdb_kaps_db_handles_t db_info[MDB_KAPS_IP_NOF_DB];
    mdb_kaps_instruction_handles_t search_instruction_info[MDB_LPM_NOF_INSTRUCTIONS];
    uint32 big_kaps_large_bb_mode[MDB_LPM_MAX_NOF_BIG_KAPS_BB_BLK_IDS];
} mdb_kaps_sw_state_t;


#endif 
