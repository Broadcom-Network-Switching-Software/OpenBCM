
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __PP_ACCESS_H__
#define __PP_ACCESS_H__

#include <soc/dnx/swstate/auto_generated/types/pp_types.h>


typedef int (*pp_db_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*pp_db_init_cb)(
    int unit);

typedef int (*pp_db_pp_stage_kbr_select_set_cb)(
    int unit, uint32 pp_stage_kbr_select_idx_0, uint32 pp_stage_kbr_select_idx_1, uint8 pp_stage_kbr_select);

typedef int (*pp_db_pp_stage_kbr_select_get_cb)(
    int unit, uint32 pp_stage_kbr_select_idx_0, uint32 pp_stage_kbr_select_idx_1, uint8 *pp_stage_kbr_select);



typedef struct {
    pp_db_pp_stage_kbr_select_set_cb set;
    pp_db_pp_stage_kbr_select_get_cb get;
} pp_db_pp_stage_kbr_select_cbs;

typedef struct {
    pp_db_is_init_cb is_init;
    pp_db_init_cb init;
    pp_db_pp_stage_kbr_select_cbs pp_stage_kbr_select;
} pp_db_cbs;





extern pp_db_cbs pp_db;

#endif 
