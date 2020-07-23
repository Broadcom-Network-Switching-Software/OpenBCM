/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#ifndef __KAPS_DIAG_H
#define __KAPS_DIAG_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/kbp_portable.h>
#include <soc/kbp/alg_kbp/include/xpt_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/error.h>


soc_error_t soc_jer_pp_diag_kaps_lkup_info_get(
   SOC_SAND_IN int      unit,
   SOC_SAND_IN int core_id
   );

int jer_kaps_sw_search_test(int unit, uint32 tbl_id, uint8 *key, uint8 *payload, int32 *prefix_len, int32 *is_matched);

int jer_kaps_hw_search_test(int unit, uint32 search_id, uint8 *master_key, struct kbp_search_result *cmp_rslt);

int jer_pp_kaps_search_test(int unit, uint32 add_entries, uint32 search_entries, uint32 delete_entries, uint32 loop_test, uint32 cache_test);

int jer_kaps_parse_print_entry(int unit, uint32 dbal_table_id, uint32 prio_len, uint8 *data_bytes, uint8 *ad_8, uint8 header_flag);

int jer_kaps_show_table(int unit, uint32 dbal_table_id, uint32 print_entries);

int jer_kaps_show_db_stats( int unit, uint32 dbal_table_id );

int jer_pp_kaps_tcam_bist(int unit);

int jer_pp_kbp_sdk_ver_test(int unit);

#endif
