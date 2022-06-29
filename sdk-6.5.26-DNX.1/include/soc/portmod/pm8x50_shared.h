
/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _PM8X50_SHARED_H_
#define _PM8X50_SHARED_H_

#define PM8X50_LANES_PER_CORE (8)
#define MAX_PORTS_PER_PM8X50 (8)
#define PM8X50_MAX_NUM_PHYS  (1)
#define PMD_INFO_DATA_STRUCTURE_SIZE     128     /* in bytes */



#define PM8X50_TS_ENTRY_SIZE            3
#define PM8X50_TS_MPP_MEM_SIZE          160
#define PM8X50_TS_400G_MEM_SIZE         40

typedef uint32_t pm_ts_table_entry[PM8X50_TS_ENTRY_SIZE];
typedef struct pm8x50_1588_lookup_memory_s {
    uint32_t tx_lkup_1588_mem_mpp0[PM8X50_TS_MPP_MEM_SIZE][PM8X50_TS_ENTRY_SIZE]; /**< TX_LKUP_1588_MEM_MPP0m */
    uint32_t tx_lkup_1588_mem_mpp1[PM8X50_TS_MPP_MEM_SIZE][PM8X50_TS_ENTRY_SIZE]; /**< TX_LKUP_1588_MEM_MPP1m */
    uint32_t tx_lkup_1588_mem_400g[PM8X50_TS_400G_MEM_SIZE][PM8X50_TS_ENTRY_SIZE]; /**< TX_LKUP_1588_MEM_400Gm */
    uint32_t rx_lkup_1588_mem_mpp0[PM8X50_TS_MPP_MEM_SIZE][PM8X50_TS_ENTRY_SIZE]; /**< RX_LKUP_1588_MEM_MPP0m */
    uint32_t rx_lkup_1588_mem_mpp1[PM8X50_TS_MPP_MEM_SIZE][PM8X50_TS_ENTRY_SIZE]; /**< RX_LKUP_1588_MEM_MPP1m */
    uint32_t rx_lkup_1588_mem_400g[PM8X50_TS_400G_MEM_SIZE][PM8X50_TS_ENTRY_SIZE]; /**< RX_LKUP_1588_MEM_400Gm */
} pm8x50_1588_lookup_memory_t;

struct pm8x50_s{
    portmod_pbmp_t phys;
    int first_phy;
    phymod_ref_clk_t ref_clk;
    phymod_polarity_t polarity;
    phymod_lane_map_t lane_map;
    phymod_firmware_load_method_t fw_load_method;
    phymod_firmware_loader_f external_fw_loader;
    phymod_core_access_t int_core_access;
    uint8 core_num;
    portmod_mac_soft_reset_f portmod_mac_soft_reset;
    phymod_afe_pll_t afe_pll;
    int warmboot_skip_db_restore;
    uint8 tvco;
    uint8 ovco;
    int rescal;
    int is_master_pm;
    portmod_egress_buffer_reset_f portmod_egress_buffer_reset;
    void* pmd_info;
    int allow_20p625g_tvco;
    int single_vco_used_by_ports;
    int clock_buffer_disable_required;
    int clock_buffer_bw_update_required;
    /* The delay from CMIC to PCS in nanoseconds. */
    uint32 pm_offset;
    pm8x50_1588_lookup_memory_t pm_1588_lookup_shadow_memory;
};

/* for a bypass port: convert speed to VCO rate */
int pm8x50_shared_pcs_bypassed_vco_get(int speed, portmod_vco_type_t* vco);

#endif /*_PM8X50_SHARED_H_*/
