/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _JER_OCB_DRAM_BUFFERS_H
#define _JER_OCB_DRAM_BUFFERS_H


#include <soc/dpp/SAND/Utils/sand_framework.h>



typedef struct soc_jer_ocb_dram_dbuf_val_s {
    uint32 start;
    uint32 end;
    uint32 size;
} soc_jer_ocb_dram_dbuf_val_t;


typedef struct jer_init_dbuffs_bdry_s {
    soc_jer_ocb_dram_dbuf_val_t ocb_0;
    soc_jer_ocb_dram_dbuf_val_t ocb_1;
    soc_jer_ocb_dram_dbuf_val_t fmc_0;
    soc_jer_ocb_dram_dbuf_val_t fmc_1;
    soc_jer_ocb_dram_dbuf_val_t mnmc_0;
    soc_jer_ocb_dram_dbuf_val_t mnmc_1;
    soc_jer_ocb_dram_dbuf_val_t fbc_fmc_0;
    soc_jer_ocb_dram_dbuf_val_t fbc_mnmc_0;
    soc_jer_ocb_dram_dbuf_val_t fbc_fmc_1;
    soc_jer_ocb_dram_dbuf_val_t fbc_mnmc_1;
} soc_jer_ocb_dram_dbuffs_bound_t;

typedef struct jer_init_dbuffs_s {
    soc_jer_ocb_dram_dbuffs_bound_t  dbuffs_bdries;
    uint32                           max_nof_dram_buffers;
} soc_jer_ocb_dram_dbuffs_t;



#define JERICHO_INIT_FMC_64_REP_512K_DBUFF_MODE 0
#define JERICHO_INIT_FMC_4K_REP_256K_DBUFF_MODE 1
#define JERICHO_INIT_FMC_NO_REP_DBUFF_MODE      2

#define JER_OCB_BANK_NUM 16
#define JER_OCB_BANK_SIZE (4 * 1024)


int soc_jer_str_prop_mc_nbr_full_dbuff_get(int unit, int *p_val);
int soc_jer_ocb_dram_buffer_conf_calc(int unit);
int soc_qax_dram_buffer_conf_calc(int unit);
int soc_jer_ocb_dram_buffer_conf_set(int unit);
int soc_qax_dram_buffer_conf_set(int unit);
int soc_jer_ocb_conf_set(int unit);
int soc_jer_ocb_dram_buffer_autogen_set(int unit);
int soc_jer_ocb_control_range_dram_mix_dbuff_threshold_set( SOC_SAND_IN int unit, SOC_SAND_IN int core, SOC_SAND_IN int range_max,   SOC_SAND_IN int range_min);
int soc_jer_ocb_control_range_dram_mix_dbuff_threshold_get( SOC_SAND_IN int unit, SOC_SAND_IN int core, SOC_SAND_OUT int* range_max, SOC_SAND_OUT int* range_min);
int soc_jer_ocb_control_range_ocb_committed_multicast_set(  SOC_SAND_IN int unit, SOC_SAND_IN uint32 index,     SOC_SAND_IN int range_max,   SOC_SAND_IN int range_min);
int soc_jer_ocb_control_range_ocb_committed_multicast_get(  SOC_SAND_IN int unit, SOC_SAND_IN uint32 index,     SOC_SAND_OUT int* range_max, SOC_SAND_OUT int* range_min);
int soc_jer_ocb_control_range_ocb_eligible_multicast_set(   SOC_SAND_IN int unit, SOC_SAND_IN uint32 index,     SOC_SAND_IN int range_max,   SOC_SAND_IN int range_min);
int soc_jer_ocb_control_range_ocb_eligible_multicast_get(   SOC_SAND_IN int unit, SOC_SAND_IN uint32 index,     SOC_SAND_OUT int* range_max, SOC_SAND_OUT int* range_min);





#endif 
