
/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _PM8X100_GEN2_SHARED_H_
#define _PM8X100_GEN2_SHARED_H_

#define PM8X100_GEN2_LANES_PER_CORE (8)
#define MAX_PORTS_PER_PM8X100_GEN2  (8)
#define PM8X100_GEN2_MAX_NUM_PHYS   (1)
#define PMD_INFO_DATA_STRUCTURE_SIZE     128     /* in bytes */


/*! PM Timesync Table Entry Size. */
#define PM8X100_GEN2_TS_ENTRY_SIZE                 3

/*! PM Tx Timesync Table Size. */
#define PM8X100_GEN2_TS_TX_MPP_MEM_SIZE          160

/*! PM Rx Timesync Table Size for U0. */
#define PM8X100_GEN2_TS_U0_RX_MPP_MEM_SIZE       160

/*! PM Rx Timesync Table Size for U1. */
#define PM8X100_GEN2_TS_U1_RX_MPP_MEM_SIZE        80

/*! PM Timesync Table Entry. */
typedef uint32_t pm_ts_table_entry[PM8X100_GEN2_TS_ENTRY_SIZE];

/*! Structure for storing PM specific 1588 table shawdow memory. */
typedef struct pm8x100_gen2_1588_lookup_memory_s {
    /*! U0 TX_LKUP_1588_MEM_MPP0m/MPPm */
    pm_ts_table_entry tx_lkup_1588_mem_mpp0[PM8X100_GEN2_TS_TX_MPP_MEM_SIZE];

    /*! U0 TX_LKUP_1588_MEM_MPP1m */
    pm_ts_table_entry tx_lkup_1588_mem_mpp1[PM8X100_GEN2_TS_TX_MPP_MEM_SIZE];

    /*! U0 RX_LKUP_1588_MEM_MPP0m */
    pm_ts_table_entry rx_lkup_1588_mem_mpp0[PM8X100_GEN2_TS_U0_RX_MPP_MEM_SIZE];

    /*! U0 RX_LKUP_1588_MEM_MPP1m */
    pm_ts_table_entry rx_lkup_1588_mem_mpp1[PM8X100_GEN2_TS_U0_RX_MPP_MEM_SIZE];

    /*! U1 TX_LKUP_1588_MEM_MPPm */
    pm_ts_table_entry tx_lkup_1588_mem_mpp_u1[PM8X100_GEN2_TS_TX_MPP_MEM_SIZE];

    /*! U1 RX_LKUP_1588_MEM_MPP0m */
    pm_ts_table_entry rx_lkup_1588_mem_mpp0_u1[PM8X100_GEN2_TS_U1_RX_MPP_MEM_SIZE];

    /*! U1 RX_LKUP_1588_MEM_MPP1m */
    pm_ts_table_entry rx_lkup_1588_mem_mpp1_u1[PM8X100_GEN2_TS_U1_RX_MPP_MEM_SIZE];

} pm8x100_gen2_1588_lookup_memory_t;

struct pm8x100_gen2_s{
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
    int warmboot_skip_db_restore;
    uint8 tvco;
    int is_master_pm;
    void* pmd_info;
    /* The delay from CMIC to PCS in nanoseconds. */
    uint32 pm_offset;
    pm8x100_gen2_1588_lookup_memory_t pm_1588_lookup_shadow_memory;
};

#endif /*_PM8X100_GEN2_SHARED_H_*/
